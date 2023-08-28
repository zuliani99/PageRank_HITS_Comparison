#include "Graph.hpp"
#include <cmath>

// Class that take into account all the computation of the PageRank algorithm
class PageRank {
	public: 
		PageRank(std::vector<unsigned int> top_k, std::string ds_path, double t_prob) : t_prob(t_prob) {
			this->top_k = top_k;
			this->graph = Graph(ds_path);
			
			for (unsigned int i = 0; i < this->graph.nodes; i++) this->PR_Prestige[i] = 1. / this->graph.nodes;

			// Create dangling nodes and cardinality map
			this->set_card_map_and_dan_node();
			
			// Create transpose matrix
			this->set_T_matrix();
		}

		// Vector that indicates the top_k value that we have to compute
		std::vector<unsigned int> top_k; 

		// Vector that store the results for each top_k
		top_k_results PR_topk; 

		// Unordered map that memorize the actual PageRank Prestige for each node
		std::unordered_map<unsigned int, double> PR_Prestige; 
		unsigned int steps = 0;
		Duration elapsed;
		
		// Private functions declaration

		void compute();
		void get_topk_results();
		void print_topk_results();
		void print_stats();
		void free_T_matrix_memory();

	private:
		Graph graph;
		const double t_prob;
		card_map cardinality_map;

		// Vector to memorize the ID of dangling nodes
		std::vector<unsigned int> dangling_nodes; 
		std::string algo_str = "PageRank Prestige"; 

		// Pointer for the transpose matrix
		traspose_pair* pt_traspose; 

		// Vector that stores the row pointer and the non empty row ID
		std::vector<std::pair<unsigned int, unsigned int>> row_pointers; 

		// Public functions declaration

		void set_card_map_and_dan_node();
		void add_danglings(unsigned int start, unsigned int end);
		void set_T_matrix();
		bool converge(std::unordered_map<unsigned int, double> &temp_Pk);
};


// Function to add a sequence of dangling nodes to the vector
void PageRank::add_danglings(unsigned int start, unsigned int end) {
	for (unsigned int dan = start + 1; dan <= end - 1; dan++)
		this->dangling_nodes.push_back(dan);
}


// Function to set the cardinality map and the vector of dangling nodes
void PageRank::set_card_map_and_dan_node(){
	unsigned int cardinality = 0;

	// Sort the sequence of pair of points by the fist element by increasingly oder
	std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.edges, compareByFirstIncreasing);

	unsigned int predecessor = this->graph.np_pointer[0].first;//-1;

	for (unsigned int i = 0; i < this->graph.edges; i++) {
		//std::cout << predecessor << " " << this->graph.np_pointer[i].first << std::endl;
		if (predecessor == this->graph.np_pointer[i].first) {
			cardinality++;
		} else {
			//if (predecessor == -1 && this->graph.edges != this->graph.max_node) {
			//	this->add_danglings(predecessor, this->graph.np_pointer[i].first);
			//} else {
			this->cardinality_map[predecessor] = cardinality;
				
			if (this->graph.np_pointer[i].first - predecessor > 1)
				this->add_danglings(predecessor, this->graph.np_pointer[i].first);
			//}
			predecessor = this->graph.np_pointer[i].first;
			cardinality = 1;
		}
	}

	// In case there are remaning nodes to view after the max node, they are all dangling nodes
 	if ((this->graph.max_node) - (this->graph.np_pointer[this->graph.edges - 1].first) > 0)
		this->add_danglings(this->graph.np_pointer[this->graph.edges - 1].first, this->graph.max_node + 1);
    
	this->cardinality_map[predecessor] = cardinality;


	/*std::cout << "Cardinality\n";
	for(int i = 0; i < this->cardinality_map.size(); i++) {
		std::cout << i << " " << this->cardinality_map[i] << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Dangling\n";
	for(int i = 0; i < this->dangling_nodes.size(); i++) {
		std::cout << this->dangling_nodes[i] << std::endl;
	}
	std::cout << std::endl;*/
}


// Function to set the transpose matrix
void PageRank::set_T_matrix() {

	// Sort the sequence of pair of points by the second element by increasingly oder
	std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.edges, compareBySecondIncreasing);

	/*std::cout << "sorting by increasing order of the second element\n";
	for(int i = 0; i < this->graph.edges; i++) {
		std::cout << this->graph.np_pointer[i].first << " " << this->graph.np_pointer[i].second << std::endl;
	}
	std::cout << std::endl;*/

	// Allocate the right amount of memory
    this->pt_traspose = (traspose_pair*)mmap(NULL, this->graph.edges * sizeof(traspose_pair), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	if (this->pt_traspose == MAP_FAILED)
        throw std::runtime_error("Mapping pt_traspose Failed\n");


	for (unsigned int j = 0; j < this->graph.edges; j++) {
		if(j == 0 || this->graph.np_pointer[j].second != this->graph.np_pointer[j - 1].second)
			// Add the row pointer and the non empty row pointer in the vector of pow pointers pairs
			this->row_pointers.push_back(std::make_pair(j, this->graph.np_pointer[j].second - this->graph.min_node));

														// 1/Oi														 col_index
		this->pt_traspose[j] = traspose_pair(1. / this->cardinality_map[this->graph.np_pointer[j].first], this->graph.np_pointer[j].first);
	}


	this->row_pointers.push_back(std::make_pair(this->graph.edges, this->graph.edges)); // Notify the conclusion of out transpose matrix
	this->graph.freeMemory(); // Free the Graph structure since now we will use only the transpose matrix


	// for(int i = 0; i < this->graph.edges; i++) {
	// 	std::cout << this->pt_traspose[i].first << " " << this->pt_traspose[i].second << std::endl;
	// }
	// std::cout << std::endl;

	// std::cout << "DEBUG\n row pointers";
	// for(int i = 0; i < this->row_pointers.size(); i++) {
	// 	std::cout << this->row_pointers[i].first << " " << this->row_pointers[i].second << std::endl;
	// }

}



// Actual function that computes the PageRank Prestige
void PageRank::compute() {

	// Initialize a temporary PageRank Prestige that will be empty after each do while iteration 
	std::unordered_map<unsigned int, double> current_PR_Prestige;
	for (unsigned int i = 0; i < this->graph.nodes; i++) current_PR_Prestige[i] = 0.;// 1. / this->graph.nodes;

	auto start = now();

	do {
		double dangling_Pk = 0.;

		// Computing the PageRank of dangling nodes
		for(int dan : this->dangling_nodes)
			dangling_Pk += this->PR_Prestige[dan - this->graph.min_node] * (1. / this->graph.nodes);

		unsigned int row_ptr = 0;
		unsigned int next_row_ptr = this->row_pointers[row_ptr + 1].first;  

		// Computing the A^t * P_k
		for(unsigned int i = 0; i < this->graph.edges; i++) {
			
			// Control to permit to update the same node until we change matrix row
			if(i == next_row_ptr){
				row_ptr++;
				next_row_ptr = this->row_pointers[row_ptr + 1].first;
			}

			// Here is done the actual matrix moltiplication between the transpose matrix #edges x #edges times the PR column vector #edges x 1
			// So the summation between all the moltiplication of the elements of a given row and the element of the column vector

			current_PR_Prestige[this->row_pointers[row_ptr].second] += this->pt_traspose[i].first * this->PR_Prestige[this->pt_traspose[i].second - this->graph.min_node];
		}

		// Computing -------------------------> (d_Pk + A^t * P_k) * d 						 +				 (1 - d) / n
		for (unsigned int i = 0; i < current_PR_Prestige.size(); i++)	
			current_PR_Prestige[i] = ((dangling_Pk + current_PR_Prestige[i]) * this->t_prob) + (1 - this->t_prob) / this->graph.nodes;

		this->steps++;

		/*std::cout << "]\n";
		std::cout << "current_PR_Prestige = [";
		for (int i = 0; i<current_PR_Prestige.size(); i++){
			std::cout << current_PR_Prestige[i] << ", ";
		}
		std::cout << "]\n\n";*/


	} while(this->converge(current_PR_Prestige)); // Check convergence

	this->elapsed = now() - start;

}


// Function to verify if we reach the point of convergence
bool PageRank::converge(std::unordered_map<unsigned int, double> &temp_Pk) {
	double distance = 0.;

	// Get the total distance of absolute difference between the actual PR Prestige vector and the early computed one
	for (unsigned int i = 0; i < temp_Pk.size(); i++) 
		distance += std::pow(std::abs(this->PR_Prestige[i] - temp_Pk[i]), 2.);

	this->PR_Prestige = temp_Pk; // Update

	// Empty the temporary vector
	temp_Pk.clear();
	for (unsigned int i = 0; i < this->graph.nodes; i++) temp_Pk[i] = 0.;//1. / this->graph.nodes;

	return std::sqrt(distance) > std::pow(10, -10); // Verify the convergence
}



// Function that frees the permanent memory regarding the transpose matrix
void PageRank::free_T_matrix_memory(){
	if (munmap(this->pt_traspose, this->graph.edges) != 0)
    	throw std::runtime_error("Free memory failed\n");
}


// Fucntion to compute the top_k nodes based on the PageRank Prestige
void PageRank::get_topk_results() {
	this->graph.get_algo_topk_results(this->PR_Prestige, this->top_k, this->PR_topk);
}


// Function to print the results
void PageRank::print_topk_results() {
	this->graph.print_algo_topk_results(this->PR_topk, this->algo_str);
}


// Function to print the elapsed time and the number of steps taken
void PageRank::print_stats() {
	std::cout << "Elapsed: " << this->elapsed.count() << " ms \t Steps: "<< this->steps << std::endl;
}

