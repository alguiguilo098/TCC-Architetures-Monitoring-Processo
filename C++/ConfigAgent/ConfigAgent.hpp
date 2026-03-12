#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


struct Config {
    int BufferSize; // Buffer size in bytes
    int NumberOfThreads; // Number of threads to use
    int TruePeriodicScriptHours; // Interval in hours for periodic scripts
    int TimeSpleepBetweenReads; // Sleep time between reads in seconds
    std::string ServerHost; // Hostname or IP address of the server
    int ServerPort; // Port number for the server
    
};

void LoadConfig(const std::string& filename, Config& config);