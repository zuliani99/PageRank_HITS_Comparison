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
		std::string algo_str = "In Degree"; 
		std::unordered_map<int, int> In_Deg_Prestige;
		Duration elapsed;

		void compute();
		void get_topk_results();
		void print_topk_results();
		void print_stats();
		
	private:
		Graph graph;
};


void InDegree::compute() {
	auto start = now();
	for(int i = 0; i < this->graph.edges; i++) {
		//std::cout << "I'm checking: (" << this->graph.np_pointer[i].first << " " << this->graph.np_pointer[i].second << ")\n"; 
		this->In_Deg_Prestige[this->graph.np_pointer[i].second] += 1;
		//std::cout << this->In_Deg_Prestige[this->graph.np_pointer[i].second] << std::endl;
	}
	this->elapsed = now() - start;
}


void InDegree::get_topk_results() {
	this->graph.get_algo_topk_results<int, int>(this->In_Deg_Prestige, this->top_k, this->IN_topk);
	this->graph.freeMemory();

}


void InDegree::print_topk_results() {
	this->graph.print_algo_topk_results<int>(this->IN_topk, this->algo_str);
}

void InDegree::print_stats() {
	std::cout << "Elapsed: " << this->elapsed.count() << " ms" << std::endl;
}