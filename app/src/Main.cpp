#include "../includes/InDegree.hpp"
#include "../includes/PageRank.hpp"
#include "../includes/HITS.hpp"
#include "../includes/Jaccard.hpp"
#include <filesystem>
#include <ctime>
#include <fstream>


int main(){
	bool verbose;

	std::cout << "--------------------------- PageRank - HITS - InDegree Comparison ---------------------------\n\n";
	std::cout << "Do you want to activate VERBOSE mode? (0/1) ";
	std::cin >> verbose;

	if (verbose != 0 && verbose != 1) throw std::invalid_argument("Please insert a correct input");

	std::vector<std::string> datasets = {"web-NotreDame.txt", "web-Stanford.txt", "web-BerkStan.txt", "web-Google.txt"};
	
	std::vector<int> top_k; 
	for (int i = 0; i<19; i++) top_k.push_back(std::pow(2,i));

	const auto p1 = std::chrono::system_clock::now();
	std::time_t today_time = std::chrono::system_clock::to_time_t(p1);
    std::string result_path = std::ctime(&today_time);
	std::filesystem::create_directory("../results/" + result_path);

	std::string csv_jaccard = "jaccard_results.csv";
	std::string csv_elapsed = "elapsed_results.csv";
	std::string csv_steps = "steps_results.csv";

	std::fstream stream_jaccard;
	std::fstream stream_elapsed;
	std::fstream stream_steps;

    stream_jaccard.open("../results/" + result_path + "/" + csv_jaccard, std::ios::out | std::ios::app);
    stream_jaccard << "dataset,top_k,ID_A,ID_H,ID_PR,PR_A,PR_H,A_H\n";

    stream_elapsed.open("../results/" + result_path + "/" + csv_elapsed, std::ios::out | std::ios::app);
    stream_elapsed << "dataset,PR,HITS,ID\n";

    stream_steps.open("../results/" + result_path + "/" + csv_steps, std::ios::out | std::ios::app);
    stream_steps << "dataset,PR,HITS\n";

	std::cout << std::endl;

	for (std::string ds : datasets) {

		if (ds == "web-BerkStan.txt") top_k.push_back(std::pow(2,19));

		std::cout << "-------------------" << ds << "---------------------" << std::endl;
		
		// InDegree
		std::cout << "IN_DEGREE" << std::endl;
		InDegree in_degree = InDegree(top_k,"../dataset/" + ds);
		in_degree.compute();
		in_degree.print_stats();
		in_degree.get_topk_results();
		if(verbose) in_degree.print_topk_results();
		std::cout << std::endl;

		// PageRank
		std::cout << "PAGE_RANK" << std::endl;
		PageRank page_rank = PageRank(top_k,"../dataset/" + ds, 0.8);
		page_rank.compute();
		page_rank.print_stats();
		page_rank.get_topk_results();
		if(verbose) page_rank.print_topk_results();
		page_rank.free_T_matrix_memory();
		std::cout << std::endl;

		// HITS
		std::cout << "HITS" << std::endl;
		HITS hits = HITS(top_k,"../dataset/" + ds);
		hits.compute();
		hits.print_stats();
		hits.get_topk_hub();
		hits.get_topk_authority();
		if(verbose) {
			std::cout << std::endl;
			hits.print_topk_hub();
			std::cout << std::endl;
			hits.print_topk_authority();
		}
		hits.free_matrices_memory();
		std::cout << std::endl;

		JaccardCoefficient jaccard = JaccardCoefficient(top_k, in_degree.IN_topk, page_rank.PR_topk, hits.authority_topk, hits.hub_topk);
		jaccard.obtain_results();
		if(verbose) jaccard.print_results();

		jaccard.save_results(stream_jaccard, ds);
    	stream_steps << ds << "," << page_rank.steps << "," << hits.steps <<"\n";
    	stream_elapsed << ds << "," << page_rank.elapsed.count() << "," << hits.elapsed.count() << "," << in_degree.elapsed.count() <<"\n";

		std::cout << "-------------------" << ds << "---------------------" << std::endl << std::endl;
	}

    stream_jaccard.close();
    stream_elapsed.close();
    stream_steps.close();

	return 0;
}