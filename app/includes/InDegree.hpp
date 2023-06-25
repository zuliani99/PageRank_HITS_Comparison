#include "Graph.hpp"
#include <algorithm>

class InDegree {
	public: 
		InDegree(std::vector<int> top_k, std::string ds_path) {
			this->top_k = top_k;
			this->graph = Graph(ds_path);
        	this->In_Deg_Prestige.resize(this->graph.nodes, 0);

			std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.nodes, compareByFirst);
			std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.nodes, compareBySecond);

		}

		std::vector<int> top_k;
		std::vector<std::pair<int, std::vector<int>>> top_k_results;
		std::vector<int> In_Deg_Prestige;
		int steps = 0;
		void compute();
		void get_topk_results();
		void print_topk_results();

		
	private:
		Graph graph;
};


void InDegree::compute() {
	for(int i = 0; i < this->graph.edges; i++) {
		this->In_Deg_Prestige[this->graph.np_pointer[i].second - this->graph.min_node] += 1;
	}
}

void InDegree::get_topk_results() {
	std::vector<int> sortedIndices = argsort(this->In_Deg_Prestige);
	for(int k : this->top_k) {
		std::vector<int> topk_result(sortedIndices.begin(), sortedIndices.begin() + k);
		this->top_k_results.push_back(std::make_pair(k, topk_result));
	}
	this->graph.freeMemory();
}

void InDegree::print_topk_results() {
	for (auto p : this->top_k_results){
		std::cout << "Top " << p.first << std::endl;
		for (auto node : p.second) std::cout << node << " ";
		std::cout << std::endl << std::endl;
	}
}