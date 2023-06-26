#include "Graph.hpp"

class PageRank {
	public: 
		PageRank(std::vector<int> top_k, std::string ds_path, double t_prob) : t_prob(t_prob) {
			this->top_k = top_k;
			this->graph = Graph(ds_path);
        	this->PR_Prestige.resize(this->graph.nodes, 1. / this->graph.nodes);

			//Create dangling nodes and cardinality map
			//Create transpose matrix
		}

		std::vector<int> top_k;
		std::vector<std::pair<int, std::vector<std::pair<int, int>>>> top_k_results;
		std::vector<double> PR_Prestige;
		int steps = 0;
		void compute();
		void get_topk_results();
		void print_topk_results();

		
	private:
		Graph graph;
		const double t_prob;
		std::vector<int> dangling_nodes;
};