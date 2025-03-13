#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <random>
#include <iterator>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <utility> // for std::pair
#include <vector>
#include <nlohmann/json.hpp>

// using namespace std;
using json = nlohmann::json;

class ErrorCodeGenerator {
public:
    // The constructor initializes the weights and the discrete distribution.
    ErrorCodeGenerator(const std::map<std::string, int>& errorCodes)
        : errorCodes_(errorCodes), gen_(std::random_device{}())
    {
        // Build the weights vector from the error codes
        for (const auto& pair : errorCodes_) {
            weights_.push_back(pair.second);
        }
        dist_ = std::discrete_distribution<>(weights_.begin(), weights_.end());
    }
    
    // This function returns the next random error code.
    int getNextErrorCode() {
        // Generate a random index based on the weights
        int randomIndex = dist_(gen_);
        // Get the corresponding error code from the map.
        auto it = std::next(errorCodes_.cbegin(), randomIndex);
        try {
            // Convert the error code string to int.
            return std::stoi(it->first);
        } catch (const std::invalid_argument& e) {
            std::cout << "Error: '" << it->first << "' is not a valid integer error code." << std::endl;
            return -1; // or handle the error appropriately
        }
    }
    
private:
    std::map<std::string, int> errorCodes_;
    std::vector<double> weights_;
    std::mt19937 gen_;
    std::discrete_distribution<> dist_;
};


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


int main(int argc, char* argv[]) {

        // Read input file from arguments --input
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " --input <input file> --queue <queue name>  --n <number of errors>" << std::endl;
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
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << input_file << std::endl;
        return EXIT_FAILURE;
    }
    json j;
    file >> j;
    if (!file) {
        std::cerr << "Error: Failed to parse " << input_file << std::endl;
        return EXIT_FAILURE;
    }

    // Populate the dictionary with the error codes
    std::map<std::string, std::map<std::string, int>> dictionary;
    for (const auto& [site_name, codes] : j.items()) {
        for (const auto& [code, count] : codes.items()) {
            dictionary[site_name][code] = count;
        }
    }

    // Extract error codes and counts for the target site
    std::map<std::string, int> errorCodes;
    if (dictionary.count(queue_name) > 0) {
        for (const auto& [code, count] : dictionary[queue_name]) {
            errorCodes[code] = count;
        }
    } else {
        std::cout << "Site not found: " << queue_name << std::endl;
        return -1;
    }

    // Create an instance of the ErrorCodeGenerator class
    ErrorCodeGenerator generator(errorCodes);
    
    // Now you can call getNextErrorCode() whenever you need a new error code.
    for (int i = 0; i < n; ++i) {
        int errorCode = generator.getNextErrorCode();
        // std::cout << "Random error code: " << errorCode << std::endl;
    }
    
    return 0;
}
