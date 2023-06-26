#ifndef _GRAPH_H
#define _GRAPH_H

#include <sys/mman.h>
#include <limits.h>
#include "./Utils.hpp"


class Graph {
	public:
		Graph() { };
		Graph(std::string ds_path) {
			this->ds_path = ds_path;
			this->set_nodes_edges(ds_path);
			this->allocate_memory();
			//std::cout << "Nodes: " << this->nodes << " - Edges: " << this->edges << " - Max Node: " << this->max_node << " - Min Node: " << this->min_node << std::endl;
		}

		
		int nodes;
		int edges;
		int min_node = INT_MAX;
		int max_node = 0;
		nodes_pair* np_pointer;

		void freeMemory();

	private:
		std::string ds_path;
		void set_nodes_edges(const std::string& ds_path);
		void allocate_memory();
		std::vector<int> parseLine(const std::string& line);
		void updateMinMaxNodes(const std::vector<int>& pair);
};

void Graph::set_nodes_edges(const std::string& ds_path) { // Read the file and get the number of nodes and edges from the description
	std::ifstream file = readDataset(this->ds_path);
	std::string line;
    for (int i = 0; i < 2; ++i) std::getline(file, line);
	
	// Read the third line of the description
	std::getline(file, line);
	file.close();

	std::istringstream line_stream(line);
    std::string str;
	
	// Extract the number of nodes and edges
	while (line_stream >> str) {
        if (str == "Nodes:") 
            line_stream >> this->nodes;
        else if (str == "Edges:")
            line_stream >> this->edges;
    }
}


std::vector<int> Graph::parseLine(const std::string& line) {
    std::istringstream line_stream(line);
    std::string str;
    std::vector<int> pair;
    
    while (std::getline(line_stream, str, '\t')) {
        pair.push_back(std::stoi(str));
    }
    
    return pair;
}


void Graph::updateMinMaxNodes(const std::vector<int>& pair) {
    if (pair[0] < this->min_node)
        this->min_node = pair[0];
    if (pair[0] > this->max_node)
        this->max_node = pair[0];
    
    if (pair[1] < this->min_node)
        this->min_node = pair[1];
    if (pair[1] > this->max_node)
        this->max_node = pair[1];
}


void Graph::allocate_memory() {
    this->np_pointer = (nodes_pair*)mmap(NULL, this->edges * sizeof(nodes_pair), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if (this->np_pointer == MAP_FAILED)
        throw std::runtime_error("Mapping Failed\n");
    
    std::ifstream file = readDataset(this->ds_path);
    std::string line;
    int i = 0;
    
    while (std::getline(file, line)) {
        if (line[0] != '#') {
            std::vector<int> pair = this->parseLine(line);
            this->updateMinMaxNodes(pair);
            
            this->np_pointer[i] = nodes_pair(pair[0], pair[1]);
            i++;
        }
    }
    file.close();
}

void Graph::freeMemory() {
	if (munmap(this->np_pointer, this->edges) != 0)
    	throw std::runtime_error("Free memory failed\n");
}

#endif