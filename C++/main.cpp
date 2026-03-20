#include "AgentMonitoring/AgentMonitoring.hpp"
#include <thread>
#define PATH_CONFIG "./confagent.conf"

// Descrição: Ponto de entrada principal do programa de monitoramento de processos
// Nome: Guilherme Almeida Lopes
// Data: 2025-01-29

int main(){
    try
    {
        // Inicia o monitoramento do agente
        AgentMonitoring* agent = new AgentMonitoring(PATH_CONFIG);
        std::cout << agent->configAgent.BufferSize << std::endl;
        std::cout << agent->configAgent.NumberOfThreads << std::endl;
        std::cout << agent->configAgent.TimeSpleepBetweenReads << std::endl;
        std::cout << agent->configAgent.ServerHost << std::endl;
        std::cout << agent->configAgent.ServerPort << std::endl;

        // Start monitoring
        std::thread monitoring_thread(&AgentMonitoring::start_monitoring, agent);
        // Start server sending data thread
        std::thread server_send_thread(&AgentMonitoring::strart_sending_data_server,agent);
        
        monitoring_thread.join();
        server_send_thread.join();
        
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