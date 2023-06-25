#include <iostream>
#include "../includes/Utils.hpp"

int main(){
	std::vector<std::string> datasets = getDatasetsTXT("../dataset");

	for (auto ds : datasets) {
		std::cout << ds << "\n";
	}

	return 0;
}