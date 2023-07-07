#include "Graph.hpp"

class InDegree {
	public: 
		InDegree(std::vector<int> top_k, std::string ds_path) {
			this->top_k = top_k;
			this->graph = Graph(ds_path);

			std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.nodes, compareByFirstIncreasing);
			std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.nodes, compareBySecondIncreasing);

			/*for(int i = 0; i < this->graph.edges; i++) {
				std::cout << this->graph.np_pointer[i].first << " " << this->graph.np_pointer[i].second << std::endl;
			}*/
		}

		std::vector<int> top_k;
		top_k_results<int> IN_topk;
		std::unordered_map<int, int> In_Deg_Prestige;
		void compute();
		void get_topk_results();
		void print_topk_results();
		
	private:
		Graph graph;
};


void InDegree::compute() {
	for(int i = 0; i < this->graph.edges; i++) {
		//std::cout << "I'm checking: (" << this->graph.np_pointer[i].first << " " << this->graph.np_pointer[i].second << ")\n"; 
		this->In_Deg_Prestige[this->graph.np_pointer[i].second] += 1;
		//std::cout << this->In_Deg_Prestige[this->graph.np_pointer[i].second] << std::endl;
	}
}

void InDegree::get_topk_results() {
	std::vector<std::pair<int, int>> in_deg_vec_pairs(this->In_Deg_Prestige.begin(), this->In_Deg_Prestige.end());
	std::sort(in_deg_vec_pairs.begin(), in_deg_vec_pairs.end(), compareBySecondDecreasing<int, int>);

	for(int k : this->top_k) {
		std::vector<std::pair<int, int>> final_top_k(in_deg_vec_pairs.begin(), in_deg_vec_pairs.begin() + k);
		this->IN_topk[k] = final_top_k;
	}
	this->graph.freeMemory();

}

void InDegree::print_topk_results() {
	for (auto p : this->IN_topk){
		std::cout << "Top " << p.first << std::endl;
		int i = 1;
		for (auto node : p.second) {
			std::cout << i << ") Node ID: " << node.first << " - In Degree value: " << node.second << std::endl;
			i++;
		}
	}
}