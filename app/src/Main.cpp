#include "../includes/InDegree.hpp"
#include "../includes/PageRank.hpp"
#include "../includes/HITS.hpp"
#include "../includes/Jaccard.hpp"

int main(){
	//std::vector<std::string> datasets = getDatasetsTXT("../dataset");
	// std::vector<std::string> datasets = {"../dataset/web-NotreDame.txt"};
	std::vector<std::string> datasets = {"../dataset/test-dataset.txt"};
	std::vector<int> top_k = {10};//, 20, 30, 40, 50};

	for (auto ds : datasets) {
		std::cout << "-------------------" << ds << "---------------------" << std::endl;
		
		// InDegree
		std::cout << "IN_DEGREE" << std::endl;
		InDegree in_degree = InDegree(top_k, ds);
		in_degree.compute();
		in_degree.print_stats();
		in_degree.get_topk_results();
		in_degree.print_topk_results();

		std::cout << std::endl;

		// PageRank
		std::cout << "PAGE_RANK" << std::endl;
		PageRank page_rank = PageRank(top_k, ds, 0.8);
		page_rank.compute();
		page_rank.print_stats();
		page_rank.get_topk_results();
		page_rank.print_topk_results();

		// HITS
		std::cout << "\nHITS" << std::endl;
		HITS hits = HITS(top_k,ds);
		hits.compute();
		hits.print_stats();
		hits.get_topk_hub();
		hits.get_topk_authority();
		hits.print_topk_hub();
		hits.print_topk_authority();
		
		std::cout << "-------------------" << ds << "---------------------" << std::endl << std::endl;
	}

	return 0;
}