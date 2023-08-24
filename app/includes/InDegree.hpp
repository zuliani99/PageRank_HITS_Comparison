#include "Graph.hpp"

// Class that takes into account all the computation of the InDegree algorithm
class InDegree {
	public: 

		// Constructor
		InDegree(std::vector<int> top_k, std::string ds_path) {
			this->top_k = top_k;
			this->graph = Graph(ds_path);

			std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.nodes, compareByFirstIncreasing);
			std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.nodes, compareBySecondIncreasing);
		}

		std::vector<int> top_k; // Vector that indicates the top_k value that we have to compute
		top_k_results<int> IN_topk; // Vector that store the results for each top_k
		std::string algo_str = "In Degree"; 
		std::unordered_map<int, int> In_Deg_Prestige; // Unordered map that memorize the actual InDegree Prestige for each node
		Duration elapsed;
		int steps = 0;

		// Private function declaration
		void compute();
		void get_topk_results();
		void print_topk_results();
		void print_stats();
		
	private:
		Graph graph;
};


// Function to compute the InDegree value of each node analysing the edges
void InDegree::compute() {
	this->steps = this->graph.edges;
	auto start = now(); // Timer start
	for(int i = 0; i < this->graph.edges; i++)
		this->In_Deg_Prestige[this->graph.np_pointer[i].second] += 1;
	
	this->elapsed = now() - start; // Timer end
}


// Fucntion to compute the top_k nodes based on the InDegree Prestige
void InDegree::get_topk_results() {
	this->graph.get_algo_topk_results<int, int>(this->In_Deg_Prestige, this->top_k, this->IN_topk);
	this->graph.freeMemory();

}


// Function to print the results
void InDegree::print_topk_results() {
	this->graph.print_algo_topk_results<int>(this->IN_topk, this->algo_str);
}


// Function to print the elapsed time
void InDegree::print_stats() {
	std::cout << "Elapsed: " << this->elapsed.count() << " ms \t Steps: "<< this->steps << std::endl;
}