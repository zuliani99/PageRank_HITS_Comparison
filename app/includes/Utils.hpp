#ifndef _UTILS_H
#define _UTILS_H

#include <fstream>
#include <string>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>

// Typedef for node pair: (from_node_id, to_node_id)
using nodes_pair = std::pair<int, int>;

// Typedef for the cardinality map: (node_id, cardinality)
using card_map = std::unordered_map<int, int>;

// Typedef for the transpose matrix (used for the sparse matrix representation): (value, from_node_id) 
using traspose_pair = std::pair<double, int>;

// Typedef for the top-k results of each algorithm, for all the values of k 
template<typename T>
using top_k_results = std::unordered_map<int, std::vector<std::pair<int, T>>>;

// Time execution statements
static auto now = std::chrono::high_resolution_clock::now;
using Duration = std::chrono::duration<double, std::milli>;

// Compares the first element of a node pair in increasing order
bool compareByFirstIncreasing(const nodes_pair& pair1, const nodes_pair& pair2) {
    return pair1.first < pair2.first;
}

// Compares the second element of a node pair in increasing order
bool compareBySecondIncreasing(const nodes_pair& pair1, const nodes_pair& pair2) {
    return pair1.second < pair2.second;
}


// Compares the second element of a node pair in decreasiong order
template<typename T, typename D>
bool compareBySecondDecreasing(const std::pair<T, D>& pair1, const std::pair<T, D>& pair2) {
    return pair1.second > pair2.second;
}

// Returns the std::vector of dataset contained in the given directory path
std::vector<std::string> getDatasetsTXT(const std::string& directoryPath) {
    std::vector<std::string> txtFileList;
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            txtFileList.push_back(entry.path().string());
        }
    }
    return txtFileList;
}


// Returns the stream istance of a given dataset path
std::ifstream readDataset(const std::string& filepath) {
    std::ifstream file;
    file.open(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    return file;
}

#endif