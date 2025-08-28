#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include "json.hpp"

using json = nlohmann::json;

// Function to decode a value from a given base to a double
double decodeFromBase(const std::string& value, int base) {
    double result = 0.0;
    for (char c : value) {
        int digit;
        if (std::isdigit(c)) {
            digit = c - '0';
        } else if (std::isalpha(c)) {
            digit = std::tolower(c) - 'a' + 10;
        } else {
            throw std::runtime_error("Invalid character in value string");
        }
        
        if (digit >= base) {
            throw std::runtime_error("Digit exceeds specified base");
        }
        
        result = result * base + digit;
    }
    return result;
}

int main(int argc, char* argv[]) {
    // Check if filename is provided
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <json_file>" << std::endl;
        return 1;
    }
    
    // Open and parse the JSON file
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }
    
    json data;
    try {
        file >> data;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return 1;
    }
    
    // Extract n and k from the keys object
    int n = data["keys"]["n"];
    int k = data["keys"]["k"];
    
    // Vectors to store the points
    std::vector<double> x_values;
    std::vector<double> y_values;
    
    // Process each point in the JSON
    for (auto& element : data.items()) {
        std::string key = element.key();
        
        // Skip the "keys" object
        if (key == "keys") continue;
        
        try {
            // The key itself is the x value (convert string key to integer)
            double x = std::stod(key);
            std::string base_str = element.value()["base"];
            std::string value_str = element.value()["value"];
            
            // Convert base string to integer
            int base = std::stoi(base_str);
            
            // Decode the y value from the specified base
            double y = decodeFromBase(value_str, base);
            
            x_values.push_back(x);
            y_values.push_back(y);
        } catch (const std::exception& e) {
            std::cerr << "Error processing point '" << key << "': " << e.what() << std::endl;
            return 1;
        }
    }
    
    // Verify we have the expected number of points
    if (x_values.size() != static_cast<size_t>(n) || y_values.size() != static_cast<size_t>(n)) {
        std::cerr << "Error: Expected " << n << " points, but found " << x_values.size() << std::endl;
        return 1;
    }
    
    // Calculate f(0) using Lagrange interpolation
    double c = 0.0;
    
    for (int i = 0; i < n; i++) {
        double term = y_values[i];
        double denominator = 1.0;
        
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            
            // Calculate the Lagrange basis polynomial at x=0
            term *= (0 - x_values[j]);
            denominator *= (x_values[i] - x_values[j]);
        }
        
        c += term / denominator;
    }
    
    // Output the constant term C (f(0))
    std::cout << c << std::endl;
    
    return 0;
}