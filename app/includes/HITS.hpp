#include "Graph.hpp"

// This class provides the implementation of the HITS algorithm.
class HITS {
	public: 
		// HITS constructor.
		HITS(std::vector<unsigned int> top_k, std::string ds_path){
			this->top_k = top_k;
			this->graph = Graph(ds_path);
			this->create_L_and_L_t();
			this->initialize_ak_hk();
		}

		// Vector containing the values of k for which the top-k is computed.
		std::vector<unsigned int> top_k;

		// Map: NodeID <-> authority score at time k.
		std::unordered_map<unsigned int, double> HITS_authority;

		// Map: NodeID <-> hub score at time k.
		std::unordered_map<unsigned int, double> HITS_hub;

		// Vector containing the final top-k authority scores.
		top_k_results authority_topk;

		// Vector containing the final top-k hub scores.
		top_k_results hub_topk;
		
		// Number of steps for convergence.
		int steps = 0;

		// Elapsed time for computation.
		Duration elapsed;

		// Public functions declaration
		
		void compute_L();
		void compute_L_t();
		void create_L_and_L_t();
		void initialize_ak_hk();
		void compute();
		void get_topk_authority();
		void get_topk_hub();
		void print_authority();
		void print_hub();
		void print_topk_authority();
		void print_topk_hub();
		void print_stats();
		void free_matrices_memory();

	private:
		// Stores the graph for which the HITS is computed.
		Graph graph;

		// Pointer to the destination nodes of the adjacency matrix L.
    	unsigned int* L_ptr;

		std::vector<std::pair<unsigned int, unsigned int>> row_ptr_nempty_L;

		// Pointer to the destination nodes of the transpose od the adjacency matrix L, L_t.
    	unsigned int* L_t_ptr;

		std::vector<std::pair<unsigned int, unsigned int>> row_ptr_nempty_L_t;
		
		// String for authority.
		std::string autority_str = "Authority"; 

		// String for hub.
		std::string hub_str = "Hub"; 

		// Private functions declaration

		bool converge(std::unordered_map<unsigned int, double> &temp_a, std::unordered_map<unsigned int,double> &temp_h);
		void normalize(std::unordered_map<unsigned int, double> &ak, std::unordered_map<unsigned int, double> &hk);
};

// Function that computes the adjacency matrix L.
void HITS::compute_L(){
	
	// allocating the right amount of space in the permanent memory to save the edges of graph as a set of pairs
	this->L_ptr = (unsigned int*)mmap(NULL, this->graph.edges * sizeof(unsigned int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);

	// checking the allocation
	if (L_ptr == MAP_FAILED)
		throw std::runtime_error("Mapping failed\n");
	
	// computing L, the adjacency matrix
	for (unsigned int i = 0; i < this->graph.edges; i++){
		if(i == 0 || this->graph.np_pointer[i - 1].first != this->graph.np_pointer[i].first)

			// adding the row pointer and the non empty row pointer in the vector of pow pointers pairs
			this->row_ptr_nempty_L.push_back(std::make_pair(i, this->graph.np_pointer[i].first - this->graph.min_node));

		this->L_ptr[i] = this->graph.np_pointer[i].second;
	}

	// notifying the conclusion 
	this->row_ptr_nempty_L.push_back(std::make_pair(this->graph.edges, this->graph.edges)); 
}

// Function that computes the transpose matrix of the adjacency matrix L, L_t.
void HITS::compute_L_t(){

	// allocating the right amount of space in the permanent memory to save the edges of graph as a set of pairs
	this->L_t_ptr = (unsigned int*)mmap(NULL, this->graph.edges * sizeof(unsigned int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	
	// checking the allocation
	if (L_t_ptr == MAP_FAILED)
		throw std::runtime_error("Mapping failed\n");
		
	// computing L_t, the transpose of the adjacency matrix
	for (unsigned int i = 0; i < this->graph.edges; i++){
		if(i == 0 || this->graph.np_pointer[i - 1].second != this->graph.np_pointer[i].second)

			// adding the row pointer and the non empty row pointer in the vector of pow pointers pairs
			this->row_ptr_nempty_L_t.push_back(std::make_pair(i, this->graph.np_pointer[i].second - this->graph.min_node));

		this->L_t_ptr[i] = this->graph.np_pointer[i].first;
	}

	// notifying the conclusion 
	this->row_ptr_nempty_L_t.push_back(std::make_pair(this->graph.edges, this->graph.edges));
}

// Function that creates L and L_t and that performs the two matrix multiplications.
void HITS::create_L_and_L_t(){

	// sorting the pairs w.r.t. the source node
	std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.edges, compareByFirstIncreasing);
	this->compute_L();

	// sorting again w.r.t. the destination node 
	std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.edges, compareBySecondIncreasing);
	this->compute_L_t();

	// freeing the memory 
	this->graph.freeMemory();
}

// Function that initializes authority and hub vectors.
void HITS::initialize_ak_hk(){
	for (unsigned int i = 0; i < this->graph.nodes; i++) this->HITS_authority[i] = 1.;
	for (unsigned int i = 0; i < this->graph.nodes; i++) this->HITS_hub[i] = 1.;
}

// Function that computes autority and hub vectors.
void HITS::compute(){
    this->steps = 0;

	// authority scores at time k+1
    std::unordered_map<unsigned int, double> temp_HITS_authority;
	for (unsigned int i = 0; i < this->graph.nodes; i++) temp_HITS_authority[i] = 1.;

	// hub scores at time k+1
	std::unordered_map<unsigned int, double> temp_HITS_hub;
	for (unsigned int i = 0; i < this->graph.nodes; i++) temp_HITS_hub[i] = 1.;

	auto start = now();

	// repeat until convergence
    do {
        this->steps++;

		// hub score
    	// h_k = L * a_k-1

        unsigned int row_ptr = 0;
        unsigned int next_row_ptr = this->row_ptr_nempty_L[row_ptr + 1].first;
        for (unsigned int i = 0; i < this->graph.edges; i++){
            if (i == next_row_ptr){
                row_ptr++;
                next_row_ptr = this->row_ptr_nempty_L[row_ptr + 1].first;
            }
			temp_HITS_hub[this->row_ptr_nempty_L[row_ptr].second] += 1 * this->HITS_authority[this->L_ptr[i]];
        }

        // authority score
		// a_k = L^t * h_k-1

        row_ptr = 0;
        next_row_ptr = this->row_ptr_nempty_L_t[row_ptr + 1].first;
        for (unsigned int i = 0; i < this->graph.edges; i++){
            if (i == next_row_ptr){
                row_ptr++;
                next_row_ptr = this->row_ptr_nempty_L_t[row_ptr + 1].first;
            }
			temp_HITS_authority[this->row_ptr_nempty_L_t[row_ptr].second] += 1 * this->HITS_hub[this->L_t_ptr[i]];
		}

        this->normalize(temp_HITS_authority, temp_HITS_hub);		
    } while (this->converge(temp_HITS_authority, temp_HITS_hub));
	
	this->elapsed = now() - start;
}

// Function that establishes whether the execution of the HITS algorithm should continue or not.
bool HITS::converge(std::unordered_map<unsigned int, double> &temp_a, std::unordered_map<unsigned int, double> &temp_h){
	double distance_a = 0.;
	double distance_h = 0.;

	for (unsigned int i = 0; i < temp_a.size(); i++) 
		distance_a += std::pow(std::abs(this->HITS_authority[i] - temp_a[i]), 2.);

	for (unsigned int i = 0; i < temp_h.size(); i++) 
		distance_h += std::pow(std::abs(this->HITS_hub[i] - temp_h[i]), 2.);

	this->HITS_authority = temp_a;
	this->HITS_hub = temp_h;

	return std::sqrt(distance_a) > std::pow(10, -10) && std::sqrt(distance_h) > std::pow(10, -10);
}

// Function that normalizes the vectors in order to obtain a probability distribution.
void HITS::normalize(std::unordered_map<unsigned int, double> &ak, std::unordered_map<unsigned int, double> &hk){
	double sum_a_k = 0.0;
	double sum_h_k = 0.0;
	
	for (unsigned int i = 0; i < this->graph.nodes; i++){
		sum_a_k += ak[i];
		sum_h_k += hk[i];
	}

	for (unsigned int i = 0; i < this->graph.nodes; i++){
		ak[i] = ak[i] / sum_a_k;
		hk[i] = hk[i] / sum_h_k;
	}
}

// Function that frees the memory from the matrices created during the computing phase.
void HITS::free_matrices_memory(){
	if (munmap(this->L_ptr, this->graph.edges) != 0)
    	throw std::runtime_error("Free memory failed\n");
	
	if (munmap(this->L_t_ptr, this->graph.edges) != 0)
    	throw std::runtime_error("Free memory failed\n");
}

// Function that gets the top-k nodes w.r.t. the authority score.
void HITS::get_topk_authority() {
	this->graph.get_algo_topk_results(this->HITS_authority, this->top_k, this->authority_topk); 
}

// Function that gets the top-k nodes w.r.t. the hub score.
void HITS::get_topk_hub() {
	this->graph.get_algo_topk_results(this->HITS_hub, this->top_k, this->hub_topk); 
}

// Function that prints the content of the authority vector.
void HITS::print_authority(){
	std::cout << "Values of a_k = [";
	for (int i = 0; i < this->HITS_authority.size(); i++){
		std::cout << HITS_authority[i] << ",";
	}
	std::cout << "]\n";
}

// Function that prints the content of the hub vector.
void HITS::print_hub(){
	std::cout << "Values of h_k = [";
	for (int i = 0; i < this->HITS_hub.size(); i++){
		std::cout << HITS_hub[i] << ",";
	}
	std::cout << "]\n";
}

// Function that prints the authority scores for the top-k nodes.
void HITS::print_topk_authority() {
	std::cout << "Authority scores" << std::endl;
	this->graph.print_algo_topk_results(this->authority_topk, this->autority_str); 
}

// Function that prints the hub scores for the top-k nodes.
void HITS::print_topk_hub() {
	std::cout << "Hub scores" << std::endl;
	this->graph.print_algo_topk_results(this->hub_topk, this->hub_str); 
}

// Function that prints the execution time and the number of steps taken by the HITS algorithm to converge.
void HITS::print_stats() {
	std::cout << "Elapsed: " << this->elapsed.count() << " ms \t Steps: "<< this->steps << std::endl;
}