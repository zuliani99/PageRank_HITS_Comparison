#ifndef _GRAPH_H
#define _GRAPH_H

#include <sys/mman.h>
#include <limits.h>
#include "./Utils.hpp"
#include <sstream>
#include <cmath>


// Class that describes the general behaviour that a graph has, with relative generic functions
class Graph {
	public:
		// Default constructor
		Graph() { };

		// Graph constructor
		Graph(std::string ds_path) {
			this->ds_path = ds_path;
			this->set_nodes_edges(ds_path);
			this->allocate_memory();
		}
        
		int nodes;
		int edges;
		int min_node = INT32_MAX;
		int max_node = 0;
		nodes_pair* np_pointer; // Declare the pointer of nodes_pair to start memorizing edges


		// Public functions declaration
		void freeMemory();

        template<typename T, typename D>
        void get_algo_topk_results(std::unordered_map<T, D> iter, std::vector<int>& top_k, top_k_results<D>& algo_topk);

        template<typename D>
        void print_algo_topk_results(top_k_results<D>& algo_topk, std::string& algo_str);

	private:
		std::string ds_path;
		std::vector<int> parseLine(const std::string& line);

		// Private functions declaration
		void set_nodes_edges(const std::string& ds_path);
		void allocate_memory();
		void updateMinMaxNodes(const std::vector<int>& pair);
};


// Function that reads the file and gets the number of nodes and edges from the description
void Graph::set_nodes_edges(const std::string& ds_path) { 
	std::ifstream file = readDataset(this->ds_path);
	std::string line;
    for (int i = 0; i < 2; ++i) std::getline(file, line);
	
	// Reading the third line of the description
	std::getline(file, line);
	file.close();

	std::istringstream line_stream(line);
    std::string str;
	
	// Extracting the number of nodes and edges
	while (line_stream >> str) {
        if (str == "Nodes:") 
            line_stream >> this->nodes;
        else if (str == "Edges:")
            line_stream >> this->edges;
    }
}


// Function that parses the string line
std::vector<int> Graph::parseLine(const std::string& line) {
    std::istringstream line_stream(line);
    std::string str;
    std::vector<int> pair;
    
    while (std::getline(line_stream, str, '\t'))
        pair.push_back(std::stoi(str));
    
    return pair;
}


// Function that updates the minimum and maximum node given a pair of nodes
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


// Function that allocates permanent memory 
void Graph::allocate_memory() {
    
	// Allocating the right amount of memory
    this->np_pointer = (nodes_pair*)mmap(NULL, this->edges * sizeof(nodes_pair), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if (this->np_pointer == MAP_FAILED)
        throw std::runtime_error("Mapping np_pointer Failed\n");
    
	// Start reading the dataset
    std::ifstream file = readDataset(this->ds_path);
    std::string line;
    int i = 0;
    
	// For each line until we pass through all the file
    while (std::getline(file, line)) {
        if (line[0] != '#') {
            std::vector<int> pair = this->parseLine(line);
            this->updateMinMaxNodes(pair);
            
            this->np_pointer[i] = nodes_pair(pair[0], pair[1]);
			// std::cout << "DEBUG: " << "i = " << i << "\nnp_pointer = " << pair[0] << " " << pair[1] << "\n";
            i++;
        }
    }
    file.close();
}


// Function that frees the permanent memory regarding the graph
void Graph::freeMemory() {
	if (munmap(this->np_pointer, this->edges) != 0)
    	throw std::runtime_error("Free memory failed\n");
}


// Generic function to obtain the top_k nodes of a given algorithm
template<typename T, typename D>
void Graph::get_algo_topk_results(std::unordered_map<T, D> iter, std::vector<int>& topk, top_k_results<D>& algo_topk) {

    std::vector<std::pair<T, D>> pairs(iter.begin(), iter.end());
	std::sort(pairs.begin(), pairs.end(), compareBySecondDecreasing<T, D>);

	for(int k : topk) {
		std::vector<std::pair<T, D>> final_top_k(pairs.begin(), pairs.begin() + k); 
		algo_topk[k] = final_top_k;
	}
}


// Generic function to print the top_k nodes of a given algorithm
template<typename D>
void Graph::print_algo_topk_results(top_k_results<D>& algo_topk, std::string& algo_str) {
	for (auto p : algo_topk){
		std::cout << "Top " << p.first << std::endl;
		int i = 1;
		for (auto node : p.second) {
			std::cout << i << ") Node ID: " << node.first << " - " + algo_str + " value: " << node.second << std::endl;
			i++;
		}
	}
}

#endif