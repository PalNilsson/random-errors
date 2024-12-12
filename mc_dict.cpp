// ==============================================
// Author: Paul Nilsson
// Email: paul.nilsson@cern.ch
// Created Date: 2024-11-04
// Description: Draw random number from dictionary distribution
// ==============================================

#include <iostream>
#include <fstream>
#include <random>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {

    //////////////////////////////////////////////////////////////////////////
    // examle of simple JSON dictionary
    //
    // json error_codes = {
    //     {"1200", 1000}, 
    //     {"1201", 100}, 
    //     {"1202", 10}
    //     };
    // Print the JSON object
    // std::cout << error_codes.dump(4) << std::endl;

    //////////////////////////////////////////////////////////////////////////
    // example of simple dictionary and how to use it to create a distribution
    // to draw random errors from
    /*

    std::unordered_map<std::string, int> error_codes = {
        {"1200", 1000},
        {"1201", 100},
        {"1202", 10}
    };

        // Extract weights into a vector
    std::vector<double> weights;
    for (const auto& pair : error_codes) {
        weights.push_back(pair.second);
    }

    // Create a random number engine
    std::random_device rd;
    std::mt19937 gen(rd());

    // Create a discrete distribution
    std::discrete_distribution<> dist(weights.begin(), weights.end());

    // Generate a random index
    int random_index = dist(gen);

    // Get the random error code
    auto it = std::next(error_codes.begin(), random_index);
    std::string random_error_code = it->first;

    std::cout << "Random error code: " << random_error_code << std::endl;
    */

    /////////////////////////////////////////////////////////////////////////////
    // How to read the dictionary from a JSON file and draw random errors from it
    // Note: we only want to create the error_codes dictionary and the random_index once

    // Read error codes from JSON file
    std::ifstream file("../data/error_codes.json");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open error_codes.json\n";
        return 1;
    }
    json j;
    file >> j;
    if (!file) {
        std::cerr << "Error: Failed to parse error_codes.json\n";
        return 1;
    }

    // Extract the keys and values from the JSON and create the dictionary as an unordered map
    std::unordered_map<std::string, int> error_codes;
    for (const auto& [key, value] : j.items()) {
        // Check if the value is an integer
        if (!value.is_number_integer()) {
            std::cerr << "Error: Value is not an integer: " << value << std::endl;
            continue; // Skip this iteration
        }
        error_codes[key] = value;
    }

    // Calculate the total weight
    int total_weight = 0;
    for (const auto& pair : error_codes) {
        total_weight += pair.second;
    }

    // Create a random number engine (Mersenne Twister with a state size of 19937 bits, seeded with a random number)
    std::random_device rd;
    std::mt19937 gen(rd());

    // Create a discrete distribution
    std::vector<double> weights;
    for (const auto& pair : error_codes) {
        weights.push_back(pair.second);
    }
    std::discrete_distribution<> dist(weights.begin(), weights.end());

    // Generate a random index
    int random_index = dist(gen);

    // Get the random error code
    auto it = std::next(error_codes.begin(), random_index);
    std::string random_error_code = it->first;

    std::cout << "Random error code: " << random_error_code << std::endl;

    return 0;
}