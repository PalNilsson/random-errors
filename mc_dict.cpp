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
#include <string>
#include <cstdlib>
#include <utility> // for std::pair
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;


// Function to parse command-line arguments
std::tuple<std::string, int, std::string> parseArguments(int argc, char* argv[]) {
    std::unordered_map<std::string, std::string> args;

    // Parse command-line arguments
    for (int i = 1; i < argc - 1; i++) {
        std::string key = argv[i];
        std::string value = argv[i + 1];
        if (key == "--input" || key == "--n" || key == "--queue") {
            args[key] = value;
            i++; // Skip next as it's a value
        }
    }

    // Extract values with validation
    if (args.find("--input") == args.end()) {
        throw std::runtime_error("Error: Missing --input argument.");
    }
    if (args.find("--n") == args.end()) {
        throw std::runtime_error("Error: Missing --n argument.");
    }
    if (args.find("--queue") == args.end()) {
        throw std::runtime_error("Error: Missing --queue argument.");
    }

    std::string input_file = args["--input"];
    std::string queue_name = args["--queue"];
    int n;

    try {
        n = std::stoi(args["--n"]);
    } catch (...) {
        throw std::runtime_error("Error: Invalid value for --n. It must be an integer.");
    }

    return {input_file, n, queue_name};
}


void generateROOTScript(const std::unordered_map<int, int>& errorCounts, const std::string& filename = "error_hist.C") {
    std::ofstream file(filename);

    if (!file) {
        std::cerr << "Error: Could not create ROOT script file!" << std::endl;
        return;
    }

    file << "#include <TH1F.h>\n";
    file << "#include <TCanvas.h>\n";
    file << "#include <TApplication.h>\n\n";

    file << "void error_hist() {\n";
    file << "    TCanvas *c1 = new TCanvas(\"c1\", \"Error Code Histogram\", 800, 600);\n";
    
    int numBins = errorCounts.size();
    int minError = errorCounts.begin()->first;
    int maxError = minError;

    for (const auto& pair : errorCounts) {
        if (pair.first > maxError) maxError = pair.first;
    }

    file << "    TH1F *h = new TH1F(\"h\", \"Error Code Distribution;Error Code;Occurrences\", "
         << numBins << ", " << minError - 0.5 << ", " << maxError + 0.5 << ");\n\n";

    for (const auto& pair : errorCounts) {
        file << "    h->Fill(" << pair.first << ", " << pair.second << ");\n";
    }

    file << "\n    h->SetFillColor(38);\n"; // Set histogram color
    file << "    h->Draw();\n";
    file << "    c1->SaveAs(\"error_hist.png\");\n"; // Save histogram as PNG
    file << "}\n";

    file.close();
    std::cout << "ROOT script '" << filename << "' has been generated.\n";
}


int main(int argc, char* argv[]) {

    /////////////////////////////////////////////////////////////////////////////
    // How to read the dictionary from a JSON file and draw random errors from it
    // Note: we only want to create the error_codes dictionary and the random_index once

    // Read input file from arguments --input
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " --input <input file> --queue <queue name>  --n <number of errors>\n";
        return 1;
    }

    std::string input_file{""};
    std::string queue_name{""};
    int n{0};
    try {
        // Assign values from function
        std::tie(input_file, n, queue_name) = parseArguments(argc, argv);
        std::cout << "Input File: " << input_file << std::endl;
        std::cout << "Number of errors: " << n << std::endl;
        std::cout << "Queue Name: " << queue_name << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Read error codes from JSON file using the input argument
    ifstream file(input_file);
    if (!file.is_open()) {
        cerr << "Error: Could not open " << input_file << endl;
        return EXIT_FAILURE;
    }
    json j;
    file >> j;
    if (!file) {
        cerr << "Error: Failed to parse " << input_file << endl;
        return EXIT_FAILURE;
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

    // Extract error codes and counts for the target site
    unordered_map<string, int> error_codes;
    if (dictionary.count(queue_name) > 0) {
        for (const auto& [code, count] : dictionary[queue_name]) {
            error_codes[code] = count;
        }
    } else {
        cout << "Site not found: " << queue_name << endl;
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

    int random_index{0};
    int error_code{0};
    std::string random_error_code{""};
    auto it = error_codes.cbegin();
    std::unordered_map<int, int> errorCounts;

    for (int i = 0; i < n; i++) {
        // Generate a random index
        random_index = dist(gen);

        // Get the random error code
        it = std::next(error_codes.cbegin(), random_index);
        random_error_code = it->first;

        // try to convert random_error_code to int
        try {
            error_code = std::stoi(random_error_code);
            errorCounts[error_code]++; // Increment the count for this error code
            //std::cout << "Random error code: " << error_code << std::endl;
        } catch (std::invalid_argument& e) {
            std::cout << "Random error code: " << random_error_code << std::endl;
        }
    }

    // Print the error codes and their counts
    std::cout << "Error Code Counts:\n";
    for (const auto& pair : errorCounts) {
        std::cout << "Error Code " << pair.first << ": " << pair.second << " occurrences\n";
    }

    // Generate ROOT script
    generateROOTScript(errorCounts);

    return EXIT_SUCCESS;
}
