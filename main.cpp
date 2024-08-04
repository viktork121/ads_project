#include "ADS_set.h"
#include <iostream>

int main() {
    // basic half-assed testing script, can be modified freely

    ADS_set<int> int_set;
    ADS_set<double> double_set;
    ADS_set<std::string> string_set;

    for (int i = 0; i < 100; ++i) {
        int_set.insert(rand());
        double_set.insert(rand() / 1000.0);
        string_set.insert(std::to_string(rand()));
    }

    std::cout << "int_set size: " << int_set.size() << std::endl;

    std::cout << "double_set size: " << double_set.size() << std::endl;

    std::cout << "string_set size: " << string_set.size() << std::endl;

    std::cout << "int_set elements: ";
    for (auto i : int_set) {
        std::cout << i << " ";
    }

    std::cout << std::endl;

    std::cout << "double_set elements: ";

    for (auto i : double_set) {
        std::cout << i << " ";
    }

    std::cout << std::endl;

    std::cout << "string_set elements: ";

    for (auto i : string_set) {
        std::cout << i << " ";
    }

    std::cout << std::endl;

    return 0;

}