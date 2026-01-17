#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct Config {
    int BufferSize;
    int NumberOfThreads;
    int TruePeriodicScriptHours;
    int TimeSpleepBetweenReads;
};

void LoadConfig(const std::string& filename, Config& config);