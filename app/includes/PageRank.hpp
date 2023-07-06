#include "Graph.hpp"
#include <cmath>

class PageRank {
	public: 
		PageRank(std::vector<int> top_k, std::string ds_path, double t_prob) : t_prob(t_prob) {
			this->top_k = top_k;
			this->graph = Graph(ds_path);
			
			for (int i = 0; i < this->graph.nodes; i++) this->PR_Prestige[i] = 1. / this->graph.nodes;

			// Create dangling nodes
			// Create cardinality map
			this->set_cardinality_dangling();
			
			// Create transpose matrix
			this->set_T_matrix();
		}

		std::vector<int> top_k;
		std::vector<std::pair<int, std::vector<std::pair<int, double>>>> top_k_results;
		std::unordered_map<int, double> PR_Prestige;
		int steps = 0;
		
		void compute();
		void get_topk_results();
		void print_topk_results();


		
	private:
		Graph graph;
		const double t_prob;
		card_map cardinality_map;
		std::vector<int> dangling_nodes;
		std::vector<int> row_ptr;
		traspose_pair* pt_traspose;
		std::vector<std::pair<int, int>> row_pointers; // vector that store the row pointer and to node id of each edge 

		void set_cardinality_dangling();
		void add_danglings(int start, int end);
		void set_T_matrix();
		bool converge(std::unordered_map<int, double> &temp_Pk);

};


void PageRank::add_danglings(int start, int end) {
	for (int dan = start + 1; dan <= end - 1; dan++)
		this->dangling_nodes.push_back(dan);
}

void PageRank::set_cardinality_dangling(){
	int predecessor = -1;
	int cardinality = 0;

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


	// Debug print
	/*std::cout << "Dangling nodes: " << std::endl;
	for(int i = 0; i < this->dangling_nodes.size(); i++) {
		std::cout << this->dangling_nodes[i] << std::endl;
	}
	std::cout << std::endl << "Cardionality map: " << std::endl;
	for (auto p : this->cardinality_map) {
		std::cout << "Node id: " << p.first << " - deg: " << p.second << std::endl;
	}*/

}


void PageRank::set_T_matrix() {
	/*for (int i = 0; i < this->graph.edges; i++)
		std::cout << this->graph.np_pointer[i].first << " " << this->graph.np_pointer[i].second << std::endl;*/

	std::stable_sort(this->graph.np_pointer, this->graph.np_pointer + this->graph.edges, compareBySecondIncreasing);

	/*std::cout << "Sorted by the second element:" << std::endl;
	for (int i = 0; i < this->graph.edges; i++)
		std::cout << this->graph.np_pointer[i].first << " " << this->graph.np_pointer[i].second << std::endl;*/

    this->pt_traspose = (traspose_pair*)mmap(NULL, this->graph.edges * sizeof(traspose_pair), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	if (this->pt_traspose == MAP_FAILED)
        throw std::runtime_error("Mapping pt_traspose Failed\n");


	for (int i = 0; i < this->graph.edges; i++) {
		//if (i != 0)
			//std::cout << this->graph.np_pointer[i].second << " " << this->graph.np_pointer[i - 1].second << std::endl;
		if(i == 0 || this->graph.np_pointer[i].second != this->graph.np_pointer[i - 1].second)
			this->row_pointers.push_back(std::make_pair(i, this->graph.np_pointer[i].second));

		//std::cout << (this->cardinality_map.find(this->graph.np_pointer[i].first) != this->cardinality_map.end()) << std::endl;

		this->pt_traspose[i] = traspose_pair(1. / this->cardinality_map[this->graph.np_pointer[i].first], this->graph.np_pointer[i].first);
		//std::cout << this->graph.edges << " " << i << " " << this->pt_traspose[i].first << " " << this->pt_traspose[i].second << std::endl;
	}

	this->row_pointers.push_back(std::make_pair(0, 0));

	/*for (int i = 0; i < this->graph.edges; i++) 
		std::cout << "1/k: " << this->pt_traspose[i].first << " - to_node_id: " << this->pt_traspose[i].second << std::endl;

	for (int i = 0; i < row_pointers.size(); i++) 
		std::cout << "row_ptr: " << this->row_pointers[i].first << " - np_pointer second: " << this->row_pointers[i].second << std::endl;*/
	
	this->graph.freeMemory();

}


void PageRank::compute() {

	std::unordered_map<int, double> temp_PR_Prestige;
	for (int i = 0; i < this->graph.nodes; i++) temp_PR_Prestige[i] = 1. / this->graph.nodes;

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

			temp_PR_Prestige[this->row_pointers[temp_row_pt].second] += this->pt_traspose[i].first * this->PR_Prestige[this->pt_traspose[i].second];
		}

		// Dangling * 1/n * P_k
        for (int dang_id : this->dangling_nodes)
            dangling_Pk += this->PR_Prestige[dang_id - this->graph.min_node] * (1. / this->graph.nodes);

		for (int i = 0; i < this->graph.edges; i++)
			temp_PR_Prestige[i] = ((dangling_Pk + temp_PR_Prestige[i]) * this->t_prob) + (1 - this->t_prob) / this->graph.nodes;

	} while(this->converge(temp_PR_Prestige));

}


bool PageRank::converge(std::unordered_map<int, double> &temp_Pk) {
	double distance = 0.;
	for (int i = 0; i < temp_Pk.size(); i++) 
		distance += std::abs(this->PR_Prestige[i] - temp_Pk[i]);

	this->PR_Prestige = temp_Pk;
	//for (int i = 0; i < temp_Pk.size(); i++)
		//this->PR_Prestige[i] = temp_Pk[i];

	temp_Pk.clear();
	for (int i = 0; i < this->graph.nodes; i++) temp_Pk[i] = 1. / this->graph.nodes;

	return distance > std::pow(10, -6);
}
 

void PageRank::get_topk_results() {
	std::vector<std::pair<int, double>> PR_Prestige_vec_pairs(this->PR_Prestige.begin(), this->PR_Prestige.end());
	std::sort(PR_Prestige_vec_pairs.begin(), PR_Prestige_vec_pairs.end(), compareBySecondDecreasing<int, double>);

	for(int k : this->top_k) {
		std::vector<std::pair<int, double>> final_top_k(PR_Prestige_vec_pairs.begin(), PR_Prestige_vec_pairs.begin() + k);
		this->top_k_results.push_back(std::make_pair(k, final_top_k));
	}
}


void PageRank::print_topk_results() {
	for (auto p : this->top_k_results){
		std::cout << "Top " << p.first << std::endl;
		int i = 1;
		for (auto node : p.second) {
			std::cout << i << ") Node ID: " << node.first << " - Page Rank Prestige value: " << node.second << std::endl;
			i++;
		}
	}
}