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

// Typedef for node pair: (from_node_id, to_node_id).
using nodes_pair = std::pair<unsigned int, unsigned int>;

// Typedef for the cardinality map: (node_id, cardinality).
using card_map = std::unordered_map<unsigned int, unsigned int>;

// Typedef for the transpose matrix (used for the sparse matrix representation): (value, from_node_id).
using traspose_pair = std::pair<double, unsigned int>;

// Typedef for the top_k results of each algorithm, for all the values of k.
using top_k_results = std::unordered_map<unsigned int, std::vector<std::pair<unsigned int, double>>>;

// Time execution statements
static auto now = std::chrono::high_resolution_clock::now;
using Duration = std::chrono::duration<double, std::milli>;

// Function that compares the first element of a node pair in increasing order.
bool compareByFirstIncreasing(const nodes_pair& pair1, const nodes_pair& pair2) {
    return pair1.first < pair2.first;
}

// Function that compares the second element of a node pair in increasing order.
bool compareBySecondIncreasing(const nodes_pair& pair1, const nodes_pair& pair2) {
    return pair1.second < pair2.second;
}

// Function that compares the second element of a node pair in decreasing order.
bool compareBySecondDecreasing(const std::pair<unsigned int, double>& pair1, const std::pair<unsigned int, double>& pair2) {
    return pair1.second > pair2.second;
}

// Function that returns the std::vector of dataset contained in the given directory path.
std::vector<std::string> getDatasetsTXT(const std::string& directoryPath) {
    std::vector<std::string> txtFileList;
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
        if (entry.is_regular_file() && entry.path().extension() == ".txt")
            txtFileList.push_back(entry.path().string());

    return txtFileList;
}

// Function that returns the stream istance of a given dataset path.
std::ifstream readDataset(const std::string& filepath) {
    std::ifstream file;
    file.open(filepath);
    if (!file.is_open())
        throw std::runtime_error("Could not open file");
    
    return file;
}

#endif