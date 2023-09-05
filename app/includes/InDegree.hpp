#include "Graph.hpp"

// This class provides the implementation of the InDegree algorithm.
class InDegree {
	public: 
		// InDegree constructor.
		InDegree(std::vector<unsigned int> top_k, std::string ds_path) {
			this->top_k = top_k;
			this->graph = Graph(ds_path);

			std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.nodes, compareByFirstIncreasing);
			std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.nodes, compareBySecondIncreasing);
		}

		// Vector that indicates the k value for which the top-k ranking is computed.
		std::vector<unsigned int> top_k; 

		// Vector that stores the top-k results of the algorithm, for each value of k.
		top_k_results IN_topk; 

		std::string algo_str = "In Degree"; 

		// Unordered map that memorizes the actual InDegree Prestige for each node.
		std::unordered_map<unsigned int, double> In_Deg_Prestige; 

		// Elapsed time
		Duration elapsed;

		// Public functions declaration

		void compute();
		void get_topk_results();
		void print_topk_results();
		void print_stats();
		
	private:
		Graph graph;
};

// Function that computes the InDegree value of each node analysing the edges.
void InDegree::compute() {
	// timer start
	auto start = now();
	for(unsigned int i = 0; i < this->graph.edges; i++)
		this->In_Deg_Prestige[this->graph.np_pointer[i].second] += 1 / (this->graph.nodes - 1);
	
	// ending the timing
	this->elapsed = now() - start; 
}

// Function that retreives the top-k nodes based on the InDegree value of each node.
void InDegree::get_topk_results() {
	this->graph.get_algo_topk_results(this->In_Deg_Prestige, this->top_k, this->IN_topk);
	this->graph.freeMemory();
}

// Function that prints the results.
void InDegree::print_topk_results() {
	this->graph.print_algo_topk_results(this->IN_topk, this->algo_str);
}

// Function that prints the elapsed time.
void InDegree::print_stats() {
	std::cout << "Elapsed: " << this->elapsed.count() << " ms" << std::endl;
}