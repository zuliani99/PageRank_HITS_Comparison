#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>

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