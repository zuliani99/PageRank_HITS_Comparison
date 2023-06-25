#include <string>
#include "./Utils.hpp"

// Read only one time the total txt? And pass the memory location to HITS and PageRank?

class Graph {
	public:
		Graph(std::string ds_path) {
			this->set_nodes_edges(ds_path);
			this->allocate_memory();
		}

	private:
		std::string ds_path;
		unsigned int nodes;
		unsigned int edges;
		unsigned int min_node;
		unsigned int max_node;

		void set_nodes_edges(std::string ds_path);
		void allocate_memory();
};

void Graph::set_nodes_edges(std::string ds_path) { // Read the file and get the number of nodes and edges from the description
	this->ds_path = ds_path;
	std::ifstream strem = readDataset(this->ds_path);

}

void Graph::allocate_memory() { // we read the txt file and store the pair + set the min and mad node at the same time
	std::ifstream strem = readDataset(this->ds_path);
}