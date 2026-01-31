#include "Collection.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
// Nome: Guilherme Almeida Lopes
// Descrição: Implementação das funções para coletar o IP do host e o tempo de boot do sistema.
// Data: 31/01/2026


/**
 * @brief Coleta o número de threads do processo com o PID fornecido e define no objeto ProcessMetrics.
 * @param metrics Referência ao objeto ProcessMetrics onde o número de threads será definido.
 * @param pid PID do processo alvo.
*/
void Collection::get_metrics_num_threads(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    // Abre o arquivo /proc/[pid]/status para ler informações do processo
    std::ifstream file("/proc/" + std::to_string(pid) + "/status");
    if (!file.is_open())
        return;

    // Lê o arquivo linha por linha para encontrar a linha que começa com "Threads:"
    std::string line;
    while (std::getline(file, line))
    {
        // Verifica se a linha começa com "Threads:"
        if (line.rfind("Threads:", 0) == 0)
        {
            // Extrai o número de threads da linha
            int threads = std::stoi(line.substr(8));
            metrics.set_num_threads(threads);
            break;
        }
    }
}
/**
 * @brief Coleta o endereço IP do host e define no objeto ProcessMetrics.
 * @param metrics Referência ao objeto ProcessMetrics onde o IP do host será definido.
 */
void Collection::get_host_ip(ProcessMetricas::ProcessMetrics &metrics)
{
    //
    struct ifaddrs *ifaddr;
    if (getifaddrs(&ifaddr) == -1)
        return;

    for (struct ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            char ip[INET_ADDRSTRLEN];

            inet_ntop(AF_INET, &(sa->sin_addr), ip, INET_ADDRSTRLEN);

            if (std::strcmp(ip, "127.0.0.1") != 0)
            {
                metrics.set_hostip(ip);
                break;
            }
        }
    }

    freeifaddrs(ifaddr);
}
/// @brief  Coleta o tempo de boot do sistema e define no objeto ProcessMetrics.
/// @param metrics  Referência ao objeto ProcessMetrics onde o tempo de boot será definido.

void Collection::get_metrics_boottime(ProcessMetricas::ProcessMetrics &metrics)
{
    std::ifstream file("/proc/stat");
    if (!file.is_open())
        return;

    // Lê o arquivo linha por linha para encontrar a linha que começa com "btime"
    std::string line;
    while (std::getline(file, line)) // ler linha por linha
    {
        std::istringstream iss(line);
        std::string key;
        long long btime;

        if (iss >> key >> btime) // tenta extrair par string + número
        {
            if (key == "btime") // encontrou btime!
            {
                // Converte o tempo de boot para um formato legível
                std::time_t t = static_cast<std::time_t>(btime);
                std::tm tm = *std::localtime(&t); // converte para horário local

                // Formata a data e hora
                std::ostringstream oss;
                oss << std::put_time(&tm, "%a  %b %d  %H:%M:%S %Y");
                // Exemplo de saída: "Saturday, 25 January 2026 23:45:12"

                metrics.set_boottime(oss.str());

                return;
            }
        }
    }
}

