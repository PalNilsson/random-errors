#include <iostream>
#include <map>
#include <vector>
#include <random>
#include <iterator>
#include <string>
#include <stdexcept>

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

int main() {
    // Example error codes with their weights.
    std::map<std::string, int> errorCodes = {
        {"404", 50},
        {"500", 30},
        {"403", 20}
    };
    
    ErrorCodeGenerator generator(errorCodes);
    
    // Now you can call getNextErrorCode() whenever you need a new error code.
    for (int i = 0; i < 10; ++i) {
        int errorCode = generator.getNextErrorCode();
        std::cout << "Random error code: " << errorCode << std::endl;
    }
    
    return 0;
}
