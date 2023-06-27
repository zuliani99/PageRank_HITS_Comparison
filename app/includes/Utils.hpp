#ifndef _UG_H
#define _UG_H

#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>

typedef std::pair<unsigned int, unsigned int> nodes_pair;
typedef std::map<unsigned int, unsigned int> card_map;



bool compareByFirst(const nodes_pair& pair1, const nodes_pair& pair2) {
    return pair1.first < pair2.first;
}

bool compareBySecond(const nodes_pair& pair1, const nodes_pair& pair2) {
    return pair1.second < pair2.second;
}


bool compare(const int& a, const int& b, const std::vector<int>& values) {
    return values[a] > values[b];
}

std::vector<int> argsort(const std::vector<int>& input) {
    std::vector<int> indices(input.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
        return compare(a, b, input);
    });

    return indices;
}



// Fucntion that return the std::vector of dataset contained in the given directory path
std::vector<std::string> getDatasetsTXT(const std::string& directoryPath) {
    std::vector<std::string> txtFileList;
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            txtFileList.push_back(entry.path().string());
        }
    }
    return txtFileList;
}


// Fucntion that return the stream istance of a given dataset path
std::ifstream readDataset(const std::string& filepath) {
    std::ifstream file;
    file.open(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    return file;
}


#endif