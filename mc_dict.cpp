// ==============================================
// Author: Paul Nilsson
// Email: paul.nilsson@cern.ch
// Created Date: 2024-11-04
// Description: Draw random number from dictionary distribution
// ==============================================

#include <iostream>
#include <fstream>
#include <map>
#include <random>
#include <unordered_map>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

int main() {

    /////////////////////////////////////////////////////////////////////////////
    // How to read the dictionary from a JSON file and draw random errors from it
    // Note: we only want to create the error_codes dictionary and the random_index once

    // Read error codes from JSON file
    ifstream file("../data/error_codes.json");
    if (!file.is_open()) {
        cerr << "Error: Could not open error_codes.json\n";
        return 1;
    }
    json j;
    file >> j;
    if (!file) {
        cerr << "Error: Failed to parse error_codes.json\n";
        return 1;
    }

    map<string, map<string, int>> dictionary;

    for (const auto& [site_name, codes] : j.items()) {
        for (const auto& [code, count] : codes.items()) {
            dictionary[site_name][code] = count;
        }
    }

    // Access and use the dictionary
    //for (const auto& [site_name, codes] : dictionary) {
    //    cout << "Site: " << site_name << endl;
    //    for (const auto& [code, count] : codes) {
    //        cout << "  Code: " << code << ", Count: " << count << endl;
    //    }
    //    cout << endl;
    //}

    // The dictionary above, is assumed to be created once only. When it needs to be used, it will be for a
    // given queue/site. At that point, it should be reduced to a simpler dictionary:

    // Specify the site name you want to extract error codes from
    string target_site_name = "SARA-MATRIX_VHIMEM"; // Replace with your desired site name

    // Extract error codes and counts for the target site
    unordered_map<string, int> error_codes;
    if (dictionary.count(target_site_name) > 0) {
        for (const auto& [code, count] : dictionary[target_site_name]) {
            error_codes[code] = count;
        }
    } else {
        cout << "Site not found: " << target_site_name << endl;
    }

    // Print the extracted error codes and counts
    //for (const auto& [code, count] : error_codes) {
    //    cout << "Error Code: " << code << ", Count: " << count << endl;
    //}

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