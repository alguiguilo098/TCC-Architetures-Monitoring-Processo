#include "AgentMonitoring/AgentMonitoring.hpp"
#define PATH_CONFIG "/home/galmeidalopes/TCC-Architetures-Monitoring-Processo/C++/confagent.conf"
int main(){
    try
    {
        // Inicia o monitoramento do agente
        AgentMonitoring* agent = new AgentMonitoring(PATH_CONFIG);
        // Start monitoring
        agent->start_monitoring();
        // Cleanup
        delete agent;
    }
    catch(const std::exception& e)
    {
        // Print any exceptions that occur
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}