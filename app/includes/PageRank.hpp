#include "Graph.hpp"

class PageRank {
	public: 
		PageRank(std::vector<int> top_k, std::string ds_path, double t_prob) : t_prob(t_prob) {
			this->top_k = top_k;
			this->graph = Graph(ds_path);
        	this->PR_Prestige.resize(this->graph.nodes, 1. / this->graph.nodes);

			this->set_cardinality_dangling();
			// Create dangling nodes
			// Create cardinality map
			// Create transpose matrix
		}

		std::vector<int> top_k;
		std::vector<std::pair<int, std::vector<std::pair<int, int>>>> top_k_results;
		std::vector<double> PR_Prestige;
		int steps = 0;
		void compute();
		void get_topk_results();
		void print_topk_results();

		void set_cardinality_dangling();
		void add_danglings(int start, int end);

		
	private:
		Graph graph;
		const double t_prob;
		card_map cardinality_map;
		std::vector<int> dangling_nodes;
};


void PageRank::add_danglings(int start, int end) {
	for (int dan = start + 1; dan <= end - 1; dan++)
		this->dangling_nodes.push_back(dan);
}

void PageRank::set_cardinality_dangling(){
	int predecessor = -1;
	int cardinality = 0;

	// TODO: check with a simple example if this works correctly

	std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.nodes, compareByFirstIncreasing);

	for (int i = 0; i < this->graph.edges; i++) {
		if (predecessor == this->graph.np_pointer[i].first) {
			cardinality++;
		}else {
			this->cardinality_map[predecessor] = cardinality;
			
			if (this->graph.np_pointer[i].first - predecessor > 1)
				this->add_danglings(predecessor, this->graph.np_pointer[i].first);

			cardinality = 0;
			predecessor = this->graph.np_pointer[i].first;
		}
	}

 	if ((this->graph.max_node) - (this->graph.np_pointer[this->graph.edges - 1].first) > 0)
		this->add_danglings(this->graph.np_pointer[this->graph.edges - 1].first, this->graph.max_node);

    this->cardinality_map[predecessor] = cardinality;
}


void PageRank::compute() {

}

void PageRank::get_topk_results() {
	
}

void PageRank::print_topk_results() {
	
}