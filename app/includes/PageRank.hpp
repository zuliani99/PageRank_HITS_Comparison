#include "Graph.hpp"
#include <cmath>

// Class that provides the implementation of the PageRank algorithm.
class PageRank {
	public: 
		// PageRank constructor.
		PageRank(std::vector<unsigned int> top_k, std::string ds_path, double t_prob) : t_prob(t_prob) {
			this->top_k = top_k;
			this->graph = Graph(ds_path);
			
			for (unsigned int i = 0; i < this->graph.nodes; i++) this->PR_Prestige[i] = 1. / this->graph.nodes;

			// computing the dangling nodes vectors and cardinality map
			this->set_card_map_and_dan_node();
			
			// computing the transpose matrix
			this->set_T_matrix();
		}

		// Vector that indicates the k value for which the top-k ranking is computed.
		std::vector<unsigned int> top_k; 

		// Vector that store the results for each top_k.
		top_k_results PR_topk; 

		// Unordered map that memorize the actual PageRank Prestige for each node.
		std::unordered_map<unsigned int, double> PR_Prestige; 

		// Number of steps.
		unsigned int steps = 0;

		// Elapsed time.
		Duration elapsed;
		
		// Public functions declaration

		void compute();
		void get_topk_results();
		void print_topk_results();
		void print_stats();
		void free_T_matrix_memory();

	private:
		Graph graph;
		const double t_prob;
		card_map cardinality_map;

		// Vector that memorizes the ID of dangling nodes.
		std::vector<unsigned int> dangling_nodes; 

		std::string algo_str = "PageRank Prestige"; 

		// Pointer to the transpose matrix.
		traspose_pair* pt_traspose; 

		// Vector that stores the row pointer and the non empty row pointer.
		std::vector<std::pair<unsigned int, unsigned int>> row_pointers; 

		// Private functions declaration

		void set_card_map_and_dan_node();
		void add_danglings(unsigned int start, unsigned int end);
		void set_T_matrix();
		bool converge(std::unordered_map<unsigned int, double> &temp_Pk);
};

// Function that adds a sequence of dangling nodes to the vector.
void PageRank::add_danglings(unsigned int start, unsigned int end) {
	for (unsigned int dan = start + 1; dan <= end - 1; dan++)
		this->dangling_nodes.push_back(dan);
}

// Function that sets the cardinality map and the vector of dangling nodes.
void PageRank::set_card_map_and_dan_node(){
	unsigned int cardinality = 0;

	// sorting the sequence of pair of points by the fist element by increasingly order
	std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.edges, compareByFirstIncreasing);

	unsigned int predecessor = this->graph.np_pointer[0].first;

	for (unsigned int i = 0; i < this->graph.edges; i++) {
		if (predecessor == this->graph.np_pointer[i].first) {
			cardinality++;
		} else {
			this->cardinality_map[predecessor] = cardinality;
				
			if (this->graph.np_pointer[i].first - predecessor > 1)
				this->add_danglings(predecessor, this->graph.np_pointer[i].first);
			predecessor = this->graph.np_pointer[i].first;
			cardinality = 1;
		}
	}

	// in case there are remaning nodes to view after the max node, they are all dangling nodes
 	if ((this->graph.max_node) - (this->graph.np_pointer[this->graph.edges - 1].first) > 0)
		this->add_danglings(this->graph.np_pointer[this->graph.edges - 1].first, this->graph.max_node + 1);
    
	this->cardinality_map[predecessor] = cardinality;
}

// Function that sets the transpose matrix.
void PageRank::set_T_matrix() {

	// sorting the sequence of pair of points by the second element by increasingly oder
	std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.edges, compareBySecondIncreasing);

	// allocating the right amount of memory
    this->pt_traspose = (traspose_pair*)mmap(NULL, this->graph.edges * sizeof(traspose_pair), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	if (this->pt_traspose == MAP_FAILED)
        throw std::runtime_error("Mapping pt_traspose Failed\n");

	for (unsigned int j = 0; j < this->graph.edges; j++) {
		if(j == 0 || this->graph.np_pointer[j].second != this->graph.np_pointer[j - 1].second)
			
			// adding the row pointer and the non empty row pointer in the vector of row pointers pairs
			this->row_pointers.push_back(std::make_pair(j, this->graph.np_pointer[j].second - this->graph.min_node));

														// 1/Oi														 col_index
		this->pt_traspose[j] = traspose_pair(1. / this->cardinality_map[this->graph.np_pointer[j].first], this->graph.np_pointer[j].first);
	}

	// notifying the conclusion of the transpose matrix
	this->row_pointers.push_back(std::make_pair(this->graph.edges, this->graph.edges)); 

	// freeing the Graph structure since now we will use only the transpose matrix
	this->graph.freeMemory(); 
}

// Function that computes the PageRank Prestige.
void PageRank::compute() {

	// initializing a temporary PageRank Prestige that will be empty after each do while iteration 
	std::unordered_map<unsigned int, double> current_PR_Prestige;

	for (unsigned int i = 0; i < this->graph.nodes; i++) current_PR_Prestige[i] = 0.;

	auto start = now();

	do {
		double dangling_Pk = 0.;

		// computing the PageRank of dangling nodes
		for(int dan : this->dangling_nodes)
			dangling_Pk += this->PR_Prestige[dan - this->graph.min_node] * (1. / this->graph.nodes);

		unsigned int row_ptr = 0;
		unsigned int next_row_ptr = this->row_pointers[row_ptr + 1].first;  

		// computing the A^t * P_k
		for(unsigned int i = 0; i < this->graph.edges; i++) {
			
			// control if we can update the same node until change matrix row change
			if(i == next_row_ptr){
				row_ptr++;
				next_row_ptr = this->row_pointers[row_ptr + 1].first;
			}

			// here is done the actual matrix moltiplication between the transpose matrix (#edges x #edges) times the PR column vector (#edges x 1)
			// so the summation between all the moltiplication of the elements of a given row and the element of the column vector
			current_PR_Prestige[this->row_pointers[row_ptr].second] += this->pt_traspose[i].first * this->PR_Prestige[this->pt_traspose[i].second - this->graph.min_node];
		}

		// computing -------------------------> (d_Pk + A^t * P_k) * d 						 +				 (1 - d) / n
		for (unsigned int i = 0; i < current_PR_Prestige.size(); i++)	
			current_PR_Prestige[i] = ((dangling_Pk + current_PR_Prestige[i]) * this->t_prob) + (1 - this->t_prob) / this->graph.nodes;

		this->steps++;
	} while(this->converge(current_PR_Prestige)); 
	this->elapsed = now() - start;
}

// Function that verifies if we reach the point of convergence.
bool PageRank::converge(std::unordered_map<unsigned int, double> &temp_Pk) {
	double distance = 0.;

	// getting the total distance of absolute difference between the actual PR Prestige vector and the early computed one
	for (unsigned int i = 0; i < temp_Pk.size(); i++) 
		distance += std::pow(std::abs(this->PR_Prestige[i] - temp_Pk[i]), 2.);

	// update
	this->PR_Prestige = temp_Pk; 

	// emptying the temporary vector
	temp_Pk.clear();

	for (unsigned int i = 0; i < this->graph.nodes; i++) temp_Pk[i] = 0.;

	// verify the convergence
	return std::sqrt(distance) > std::pow(10, -10); 
}

// Function that frees the permanent memory regarding the transpose matrix.
void PageRank::free_T_matrix_memory(){
	if (munmap(this->pt_traspose, this->graph.edges) != 0)
    	throw std::runtime_error("Free memory failed\n");
}

// Function that computes the top_k nodes based on the PageRank Prestige.
void PageRank::get_topk_results() {
	this->graph.get_algo_topk_results(this->PR_Prestige, this->top_k, this->PR_topk);
}

// Function that prints the results.
void PageRank::print_topk_results() {
	this->graph.print_algo_topk_results(this->PR_topk, this->algo_str);
}

// Function that prints the elapsed time and the number of steps taken.
void PageRank::print_stats() {
	std::cout << "Elapsed: " << this->elapsed.count() << " ms \t Steps: "<< this->steps << std::endl;
}
