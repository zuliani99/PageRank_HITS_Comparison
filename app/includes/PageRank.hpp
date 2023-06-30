#include "Graph.hpp"

class PageRank {
	public: 
		PageRank(std::vector<int> top_k, std::string ds_path, double t_prob) : t_prob(t_prob) {
			this->top_k = top_k;
			this->graph = Graph(ds_path);
        	this->PR_Prestige.resize(this->graph.nodes, 1. / this->graph.nodes);

			// Create dangling nodes
			// Create cardinality map
			this->set_cardinality_dangling();
			
			// Create transpose matrix
			this->set_T_matrix();
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
		void set_T_matrix();

		
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

	std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.nodes, compareByFirstIncreasing);

	for (int i = 0; i < this->graph.edges; i++) {
		if (predecessor == this->graph.np_pointer[i].first) {
			cardinality++;
		} else {
			if (predecessor == -1) {
				this->add_danglings(predecessor, this->graph.np_pointer[i].first);
			} else {
				this->cardinality_map[predecessor] = cardinality;
				
				if (this->graph.np_pointer[i].first - predecessor > 1)
					this->add_danglings(predecessor, this->graph.np_pointer[i].first);
			}
			predecessor = this->graph.np_pointer[i].first;
			cardinality = 1;
		}
	}

 	if ((this->graph.max_node) - (this->graph.np_pointer[this->graph.edges - 1].first) > 0)
		this->add_danglings(this->graph.np_pointer[this->graph.edges - 1].first, this->graph.max_node + 1);

    this->cardinality_map[predecessor] = cardinality;


	// Debug print
	std::cout << "Dangling nodes: " << std::endl;
	for(int i = 0; i < this->dangling_nodes.size(); i++) {
		std::cout << this->dangling_nodes[i] << std::endl;
	}
	std::cout << std::endl << "Cardionality map: " << std::endl;
	for (auto p : this->cardinality_map) {
		std::cout << "Node id: " << p.first << " - deg: " << p.second << std::endl;
	}

}


void PageRank::set_T_matrix() {
	std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.nodes, compareBySecondIncreasing);

	for (int i = 0; i < this->graph.edges; i++) {

	}
}


void PageRank::compute() {

}

void PageRank::get_topk_results() {
	
}

void PageRank::print_topk_results() {
	
}