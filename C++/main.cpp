#include "./ConfigAgent/ConfigAgent.hpp"

int main()
{
    Config config;
    LoadConfig("/home/galmeidalopes/TCC-Architetures-Monitoring-Processo/C++/confagent.conf", config);

    // Use the loaded config values
    std::cout << "BufferSize: " << config.BufferSize << std::endl;
    std::cout << "NumberOfThreads: " << config.NumberOfThreads << std::endl;
    std::cout << "TruePeriodicScriptHours: " << config.TruePeriodicScriptHours << std::endl;
    std::cout << "TimeSpleepBetweenReads: " << config.TimeSpleepBetweenReads << std::endl;
    return 0;
}