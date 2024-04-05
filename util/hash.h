#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

std::unordered_map<std::string, std::vector<std::vector<int>>> categorizeIdenticalArrays(const std::vector<std::vector<int>>& arrays) {
    std::unordered_map<std::string, std::vector<std::vector<int>>> categorizedArrays;
    for (const auto& array : arrays) {
        std::string arrayKey;
        for (const auto& element : array) {
            arrayKey += std::to_string(element) + ",";
        }
        if (categorizedArrays.find(arrayKey) != categorizedArrays.end()) {
            categorizedArrays[arrayKey].push_back(array);
        } else {
            categorizedArrays[arrayKey] = {array};
        }
    }
    return categorizedArrays;
}

int main() {
    std::vector<std::vector<int>> arrays = {
        {1, 2, 3},
        {4, 5, 6},
        {1, 2, 3},
        {7, 8, 9},
        {4, 5, 6}
    };

    auto categorizedArrays = categorizeIdenticalArrays(arrays);

    for (const auto& entry : categorizedArrays) {
        std::cout << "Identical Arrays: ";
        for (const auto& array : entry.second) {
            std::cout << "[";
            for (const auto& element : array) {
                std::cout << element << " ";
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }

    return 0;
}