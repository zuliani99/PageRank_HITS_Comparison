#include "../includes/InDegree.hpp"
#include "../includes/PageRank.hpp"
#include "../includes/HITS.hpp"
#include "../includes/Jaccard.hpp"

int main(){
	std::vector<std::string> datasets = {"../dataset/web-BerkStan.txt", "../dataset/web-Google.txt", "../dataset/web-NotreDame.txt", "../dataset/web-BerkStan.txt"};
	//std::vector<std::string> datasets = {"../dataset/test-dataset.txt"};
	// std::vector<std::string> datasets = {"../dataset/web-BerkStan.txt"};
	//std::vector<std::string> datasets = {"../dataset/test-dataset_copy.txt"};
	std::vector<int> top_k = {5};//,10, 20, 30, 40};

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
		page_rank.free_T_matrix_memory();
		std::cout << std::endl;

		// HITS
		std::cout << "HITS" << std::endl;
		HITS hits = HITS(top_k,ds);
		hits.compute();
		hits.print_stats();
		hits.get_topk_hub();
		hits.get_topk_authority();
		std::cout << std::endl;
		hits.print_topk_hub();
		std::cout << std::endl;
		hits.print_topk_authority();
		hits.free_matrices_memory();
		std::cout << std::endl;

		// JaccardCoefficient jaccard = JaccardCoefficient(top_k, in_degree.IN_topk, page_rank.PR_topk, hits.authority_topk, hits.hub_topk);
		// jaccard.obtain_results();
		// jaccard.print_results();
		
		std::cout << "-------------------" << ds << "---------------------" << std::endl << std::endl;
	}

	return 0;
}