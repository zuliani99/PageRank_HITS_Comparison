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
#include <bits/stdc++.h>

// typedef for a pair of node: (from_node_id, to_node_id)
using nodes_pair = std::pair<int, int>;

// typedef for the cardinality map: (node_id, cardinality)
using card_map = std::map<int, int>;

// typedef for the transpose matrix: (value, from_node_id)
using traspose_pair = std::pair<double, int>;

// typedef for the result of each algorithm for all top_k
template<typename T>
using top_k_results = std::map<int, std::vector<std::pair<int, T>>>;

// time execution satements
static auto now = std::chrono::high_resolution_clock::now;
using Duration = std::chrono::duration<double, std::milli>;


bool compareByFirstIncreasing(const nodes_pair& pair1, const nodes_pair& pair2) {
    return pair1.first < pair2.first;
}

bool compareBySecondIncreasing(const nodes_pair& pair1, const nodes_pair& pair2) {
    return pair1.second < pair2.second;
}

template<typename T, typename D>
bool compareBySecondDecreasing(const std::pair<T, D>& pair1, const std::pair<T, D>& pair2) {
    return pair1.second > pair2.second;
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