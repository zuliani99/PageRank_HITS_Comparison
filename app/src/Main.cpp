#include "../includes/InDegree.hpp"
#include "../includes/PageRank.hpp"
//#include "../includes/HITS.hpp"

int main(){
	//std::vector<std::string> datasets = getDatasetsTXT("../dataset");
	//std::vector<std::string> datasets = {"../dataset/web-Stanford.txt"};
	std::vector<std::string> datasets = {"../dataset/test-dataset.txt"};
	std::vector<int> top_k = {10};//, 20, 30, 40, 50};

	for (auto ds : datasets) {
		std::cout << "-------------------" << ds << "---------------------" << std::endl;
		std::cout << "IN DEGREE" << std::endl;
		InDegree in_degree = InDegree(top_k, ds);
		in_degree.compute();
		in_degree.get_topk_results();
		in_degree.print_topk_results();


		// PageRank


		// HITS

		
		std::cout << "----------------------------------------" << std::endl << std::endl;
	}

	return 0;
}