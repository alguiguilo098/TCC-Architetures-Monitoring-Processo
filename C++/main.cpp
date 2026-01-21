#include "AgentMonitoring/AgentMonitoring.hpp"
#define PATH_CONFIG "/home/galmeidalopes/TCC-Architetures-Monitoring-Processo/C++/confagent.conf"
int main(){
    try
    {
        AgentMonitoring* agent = new AgentMonitoring(PATH_CONFIG);
        agent->start_monitoring();
        delete agent;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}