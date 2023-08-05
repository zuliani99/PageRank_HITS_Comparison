#include "Graph.hpp"
#include <cmath>

class PageRank {
	public: 
		PageRank(std::vector<int> top_k, std::string ds_path, double t_prob) : t_prob(t_prob) {
			this->top_k = top_k;
			this->graph = Graph(ds_path);
			
			for (int i = 0; i < this->graph.nodes; i++) this->PR_Prestige[i] = 1. / this->graph.nodes;

			// Create dangling nodes and cardinality map
			this->set_card_map_and_dan_node();
			
			// Create transpose matrix
			this->set_T_matrix();
		}

		std::vector<int> top_k;
		top_k_results<double> PR_topk;
		std::unordered_map<int, double> PR_Prestige;
		int steps = 0;
		Duration elapsed;
		
		void compute();
		void get_topk_results();
		void print_topk_results();
		void print_stats();

	private:
		Graph graph;
		const double t_prob;
		card_map cardinality_map;
		std::vector<int> dangling_nodes;
		//std::vector<int> row_ptr;
		traspose_pair* pt_traspose;

		// vector that store the row pointer and to node id of each edge 
		std::vector<std::pair<int, int>> row_pointers; 
		//std::vector<int> row_pointers; 

		void set_card_map_and_dan_node();
		void add_danglings(int start, int end);
		void set_T_matrix();
		bool converge(std::unordered_map<int, double> &temp_Pk);

};


// OK
void PageRank::add_danglings(int start, int end) {
	for (int dan = start + 1; dan <= end - 1; dan++)
		this->dangling_nodes.push_back(dan);
}


// OK
void PageRank::set_card_map_and_dan_node(){
	int predecessor = -1;
	int cardinality = 0;

	// Sort the sequence of pair of points by the fist element by increasingly oder
	std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.edges, compareByFirstIncreasing);

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


	std::cout << "Cardinality\n";
	for(int i = 0; i < this->cardinality_map.size(); i++) {
		std::cout << i << " " << this->cardinality_map[i] << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Dangling\n";
	for(int i = 0; i < this->dangling_nodes.size(); i++) {
		std::cout << this->dangling_nodes[i] << std::endl;
	}
	std::cout << std::endl;
}



void PageRank::set_T_matrix() {

	// Sort the sequence of pair of points by the second element by increasingly oder
	std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.edges, compareBySecondIncreasing);
	std::cout << "sorting by increasing order of the second element\n";
	for(int i = 0; i < this->graph.edges; i++) {
		std::cout << this->graph.np_pointer[i].first << " " << this->graph.np_pointer[i].second << std::endl;
	}
	std::cout << std::endl;

    this->pt_traspose = (traspose_pair*)mmap(NULL, this->graph.edges * sizeof(traspose_pair), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	if (this->pt_traspose == MAP_FAILED)
        throw std::runtime_error("Mapping pt_traspose Failed\n");


	for (int j = 0; j < this->graph.edges; j++) {

		if(j == 0 || this->graph.np_pointer[j].second != this->graph.np_pointer[j - 1].second)
			this->row_pointers.push_back(std::make_pair(j, this->graph.np_pointer[j].second));
											
											// 1/Oi														 col_index
		this->pt_traspose[j] = traspose_pair(1. / this->cardinality_map[this->graph.np_pointer[j].first], this->graph.np_pointer[j].first);
	}


	this->row_pointers.push_back(std::make_pair(-1, -1)); // to notify the conclusion of out transpose matrix
	this->graph.freeMemory();

	for(int i = 0; i < this->graph.edges; i++) {
		std::cout << this->pt_traspose[i].first << " " << this->pt_traspose[i].second << std::endl;
	}
	std::cout << std::endl;

	for(int i = 0; i < this->row_pointers.size(); i++) {
		std::cout << this->row_pointers[i].first << " " << this->row_pointers[i].second << std::endl;
	}

}




void PageRank::compute() {

	std::unordered_map<int, double> current_PR_Prestige;
	for (int i = 0; i < this->graph.nodes; i++) current_PR_Prestige[i] = 1. / this->graph.nodes;

	auto start = now();

	do {
		double dangling_Pk = 0.;
		// Page rank of dangling nodes
		for(int dan : this->dangling_nodes){
			dangling_Pk += this->PR_Prestige[dan] / this->graph.edges; // - this->graph.min_node
		}

		int row_ptr = 0;
		int new_row_ptr = this->row_pointers[1].first;  

		// A^t * P_k
		for(int i = 0; i < this->graph.edges; i++) {
			if(i == new_row_ptr){
				row_ptr++;
				new_row_ptr = this->row_pointers[row_ptr + 1].first;
			}
			// I have to update the same untill I change row, I'm considereing the to node id
			current_PR_Prestige[this->row_pointers[row_ptr].second] += this->pt_traspose[i].first * this->PR_Prestige[this->pt_traspose[i].second];
		}


		this->steps++;

	} while(this->converge(current_PR_Prestige));

	this->elapsed = now() - start;

}


// OK
bool PageRank::converge(std::unordered_map<int, double> &temp_Pk) {
	double distance = 0.;

	for (int i = 0; i < temp_Pk.size(); i++) 
		distance += std::abs(this->PR_Prestige[i] - temp_Pk[i]);

	this->PR_Prestige = temp_Pk;

	temp_Pk.clear();
	for (int i = 0; i < this->graph.nodes; i++) temp_Pk[i] = 1. / this->graph.nodes;

	return distance > std::pow(10, -6);
}

 

void PageRank::get_topk_results() {
	std::vector<std::pair<int, double>> PR_Prestige_vec_pairs(this->PR_Prestige.begin(), this->PR_Prestige.end());
	std::sort(PR_Prestige_vec_pairs.begin(), PR_Prestige_vec_pairs.end(), compareBySecondDecreasing<int, double>);

	for(int k : this->top_k) {
		std::vector<std::pair<int, double>> final_top_k(PR_Prestige_vec_pairs.begin(), PR_Prestige_vec_pairs.begin() + k);
		this->PR_topk[k] = final_top_k;
	}
}


// OK
void PageRank::print_topk_results() {
	for (auto p : this->PR_topk){
		std::cout << "Top " << p.first << std::endl;
		int i = 1;
		for (auto node : p.second) {
			std::cout << i << ") Node ID: " << node.first << " - Page Rank Prestige value: " << node.second << std::endl;
			i++;
		}
	}
}


// OK
void PageRank::print_stats() {
	std::cout << "Elapsed: " << this->elapsed.count() << " ms \t Steps: "<< this->steps << std::endl;
}






/*

// CAPIRE SE VA BENE E MODIFICARE IN CASO
void PageRank::compute() {

	std::unordered_map<int, double> current_PR_Prestige;
	for (int i = 0; i < this->graph.nodes; i++) current_PR_Prestige[i] = 1. / this->graph.nodes;

	auto start = now();

	do {
		double dangling_Pk = 0.;
		int temp_row_pt = 0;
		int temp_nexr_row_pt = this->row_pointers[1].first;

		this->steps++;

		// A^T * P_k
		for (int i = 0; i < this->graph.edges; i++) {
			if (i == temp_nexr_row_pt) {
				temp_row_pt++;
				temp_nexr_row_pt = this->row_pointers[temp_row_pt + 1].first;
			}

			current_PR_Prestige[this->row_pointers[temp_row_pt].second] += this->pt_traspose[i].first * this->PR_Prestige[this->pt_traspose[i].second];
		}

		// Dangling * 1/n * P_k
        for (int dang_id : this->dangling_nodes)
            dangling_Pk += this->PR_Prestige[dang_id - this->graph.min_node] * (1. / this->graph.nodes);

		for (int i = 0; i < this->graph.edges; i++)
			current_PR_Prestige[i] = ((dangling_Pk + current_PR_Prestige[i]) * this->t_prob) + (1 - this->t_prob) / this->graph.nodes;

	} while(this->converge(current_PR_Prestige));

	this->elapsed = now() - start;

}
*/
