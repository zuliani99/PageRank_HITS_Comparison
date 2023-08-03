#ifndef _JACCARD_H
#define _JACCARD_H

#include "./Utils.hpp"

class JaccardCoefficient {
	public: 
		JaccardCoefficient(std::vector<int> topk, top_k_results<int> &ID_topk, top_k_results<double> &PR_topk, top_k_results<double> &HITS_authority_topk, top_k_results<double> &HITS_hub_topk) {
			// std::cout << ID_topk[10].size() << "\n";
			// std::cout << PR_topk[10].size() << "\n";
			// std::cout << HITS_authority_topk[10].size() << "\n";
			// std::cout << HITS_hub_topk[10].size() << "\n";
			this->topk = topk;
			this->nodes_ID_topk = this->get_first_element_vector<int>(ID_topk);
			this->nodes_PR_topk = this->get_first_element_vector<double>(PR_topk);
			this->nodes_HITS_authority_topk = this->get_first_element_vector<double>(HITS_authority_topk);
			this->nodes_HITS_hub_topk = this->get_first_element_vector<double>(HITS_hub_topk);			
		}
		void obtain_results();
		void print_results();

	private:
		std::map<int, std::vector<int>> nodes_ID_topk;
		std::map<int, std::vector<int>> nodes_PR_topk;
		std::map<int, std::vector<int>> nodes_HITS_authority_topk;
		std::map<int, std::vector<int>> nodes_HITS_hub_topk;
		std::vector<int> topk;

		// 		<k		,	<'Algo1_Algo2', res		>>
		std::map<int, std::vector<std::pair<std::string, double>>> jaccard_results;

		template<typename T>
		std::map<int, std::vector<int>> get_first_element_vector(top_k_results<T> &topk_vector);

		std::vector<int> intersection(std::vector<int> &v1, std::vector<int> &v2);
		double jaccard_distance(std::vector<int> &v1, std::vector<int> &v2);
};

// Function that returns the map for each k composed by the first element of each pair
template<typename T>
std::map<int, std::vector<int>> JaccardCoefficient::get_first_element_vector(top_k_results<T> &topk_vector) {
	// For each value of k, we store the IDs of the top-k nodes
	std::map<int, std::vector<int>> jaccard_results;

	for(int k : this->topk) {
		std::vector<int> firstElements(k);

		std::transform(topk_vector[k].begin(), topk_vector[k].end(), firstElements.begin(),
						[](const std::pair<int, T> &pair) {
							return pair.first;
						});
		jaccard_results[k] = firstElements;
	}
	return jaccard_results;
}

std::vector<int> JaccardCoefficient::intersection(std::vector<int> &v1, std::vector<int> &v2) {
    std::vector<int> intersect;

	std::sort(v1.begin(), v1.end());
	std::sort(v2.begin(), v2.end());
 
    // Find the intersection of the two sets
    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), 
			std::inserter(intersect, intersect.begin()));
 
    return intersect;
}

// Function that returns the Jaccard index of two sets
double JaccardCoefficient::jaccard_distance(std::vector<int> &v1, std::vector<int> &v2) {
    // Get the intersection Size set
    double size_in = this->intersection(v1, v2).size();
 
    // Return the Jaccard Distance using the formula and the Jaccard Index
    return 1 - (size_in / (v1.size() + v2.size() - size_in));
}

void JaccardCoefficient::obtain_results() {
	for (int k : this->topk) {
		std::vector<std::pair<std::string, double>> temp_res;

		// ID_HITS
		temp_res.push_back(std::make_pair<std::string, double>("InDegree vs HITS (authority)", this->jaccard_distance(this->nodes_ID_topk[k], this->nodes_HITS_authority_topk[k])));
		temp_res.push_back(std::make_pair<std::string, double>("InDegree vs HITS (hub)", this->jaccard_distance(this->nodes_ID_topk[k], this->nodes_HITS_hub_topk[k])));
	
		// ID_PR
		temp_res.push_back(std::make_pair<std::string, double>("InDegree vs PageRank", this->jaccard_distance(this->nodes_ID_topk[k], this->nodes_PR_topk[k])));

		// PR_HITS
		temp_res.push_back(std::make_pair<std::string, double>("PageRank vs HITS (authority)", this->jaccard_distance(this->nodes_PR_topk[k], this->nodes_HITS_authority_topk[k])));
		temp_res.push_back(std::make_pair<std::string, double>("PageRank vs HITS (hub)", this->jaccard_distance(this->nodes_PR_topk[k], this->nodes_HITS_hub_topk[k])));

		this->jaccard_results[k] = temp_res;
	}
}

void JaccardCoefficient::print_results() {
	for(auto pair1 : this->jaccard_results) {
		std::cout << "TOP " << pair1.first;
		for(auto result : pair1.second)
			std::cout << "\t" << result.first << ": " << result.second << std::endl; 
		std::cout << std::endl;
	}
}

#endif