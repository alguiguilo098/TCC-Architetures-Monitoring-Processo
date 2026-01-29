#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "ConfigAgent.hpp"

// Descrição: Função para carregar a configuração do agente a partir de um arquivo
// Nome: Guilherme Almeida Lopes
// Data: 2025-01-29

// Helper function to trim whitespace from both ends of a string
// Used in LoadConfig function
static inline std::string trim(std::string s)
{
    // Remove leading and trailing whitespace
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch) { return !std::isspace(ch); }));
    // Remove trailing whitespace
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());

    return s;
}
// Function to load configuration from a file into a Config struct
/// Expects the file to have key=value pairs, one per line
void LoadConfig(const std::string& filename, Config& config)
{
    // Open the configuration file
    std::ifstream file(filename);
    if (!file)
    {
        // If the file cannot be opened, print an error and return
        std::cerr << "Error opening config file: " << filename << '\n';
        return;
    }
    // Read the file line by line
    std::string line;
    while (std::getline(file, line))
    {
        // Trim whitespace
        line = trim(line);
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#')
            continue;
        // Find the position of the '=' character 
        auto pos = line.find('=');
        if (pos == std::string::npos)
            continue;
        // Split the line into key and value
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));

        try
        {
            if (key == "BufferSize")
                config.BufferSize = std::stoi(value);
            else if (key == "NumberOfThreads")
                config.NumberOfThreads = std::stoi(value);
            else if (key == "TruePeriodicScriptHours")
                config.TruePeriodicScriptHours = std::stoi(value);
            else if (key == "TimeSpleepBetweenReads")
                config.TimeSpleepBetweenReads = std::stoi(value);
            else
                std::cerr << "Unknown config key: " << key << '\n';
        }
        catch (const std::exception& e)
        {
            std::cerr << "Invalid value for key '" << key
                      << "': " << value << '\n';
        }
    }
}
