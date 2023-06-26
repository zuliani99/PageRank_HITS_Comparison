#include <iostream>
#include "../includes/Utils.hpp"
#include "../includes/InDegree.hpp"
//#include "../includes/HITS.hpp"
//#include "../includes/PageRank.hpp"

int main(){
	//std::vector<std::string> datasets = getDatasetsTXT("../dataset");
	std::vector<std::string> datasets = {"../dataset/web-Stanford.txt"};
	std::vector<int> top_k = {10};//, 20, 30, 40, 50};

	for (auto ds : datasets) {
		std::cout << "-------------------" << ds << "---------------------" << std::endl;
		InDegree in_degree = InDegree(top_k, ds);
		in_degree.compute();
		in_degree.get_topk_results();
		in_degree.print_topk_results();
		std::cout << "----------------------------------------" << std::endl << std::endl;
	}

	return 0;
}