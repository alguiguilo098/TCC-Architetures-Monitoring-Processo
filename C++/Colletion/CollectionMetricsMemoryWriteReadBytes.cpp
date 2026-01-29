#include "Collection.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h> // sysconf

/**
 * @brief Coleta a porcentagem de uso de memória de um processo específico.
 * @param metrics Referência ao objeto ProcessMetrics onde o valor será armazenado.
 * @param pid ID do processo alvo.
 */
void Collection::get_metrics_memory_percent(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    try
    {
        // Utiliza o comando ps para obter a porcentagem de uso de memória do processo.
        std::string cmd = "ps -p " + std::to_string(pid) + " -o %mem=";

        FILE *pipe = popen(cmd.c_str(), "r");
        if (!pipe)
        {
            // Se não for possível abrir o pipe, define memory_percent como 0.
            metrics.set_memory_percent(0.0f);
            return;
        }

        // Lê a saída do comando.
        char buffer[128];
        std::string result;

        if (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            // Remove espaços em branco e nova linha.
            result = buffer;
        }

        // Fecha o pipe.
        pclose(pipe);

        if (result.empty())
        {
            // Se a saída estiver vazia, define memory_percent como 0.
            metrics.set_memory_percent(0.0f);
            return;
        }

        // Converte a string para float e define no objeto metrics.
        float mem_percent = std::stof(result);
        metrics.set_memory_percent(mem_percent);
    }
    catch (...)
    {
        metrics.set_memory_percent(0.0f);
    }
}

/**
 * @brief Coleta a quantidade de bytes lidos por um processo específico.
 * @param metrics Referência ao objeto ProcessMetrics onde o valor será armazenado.
 * @param pid ID do processo alvo.
 */

void Collection::get_metrics_read_bytes(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    // Coleta a quantidade de bytes lidos por um processo específico.
    std::ifstream io_file("/proc/" + std::to_string(pid) + "/io");
    if (!io_file.is_open())
    {
        // Se não for possível abrir o arquivo, define read_bytes como 0.
        metrics.set_read_bytes(0);
        return;
    }

    // Lê o arquivo linha por linha.
    std::string line;
    while (std::getline(io_file, line))
    {
        // Procura pela linha que contém "read_bytes:"
        if (line.find("read_bytes:") == 0)
        {
            std::istringstream iss(line);
            std::string label;
            uint64_t read_bytes;
            iss >> label >> read_bytes;
            metrics.set_read_bytes(read_bytes);
            return;
        }
    }

    metrics.set_read_bytes(0);
}

/**
 * @brief Coleta a quantidade de bytes escritos por um processo específico.
 * @param metrics Referência ao objeto ProcessMetrics onde o valor será armazenado.
 * @param pid ID do processo alvo.
 */
void Collection::get_metrics_write_bytes(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    // Coleta a quantidade de bytes escritos por um processo específico.
    std::ifstream io_file("/proc/" + std::to_string(pid) + "/io");
    if (!io_file.is_open())
    {
        // Se não for possível abrir o arquivo, define write_bytes como 0.
        metrics.set_write_bytes(0);
        return;
    }

    std::string line;
    while (std::getline(io_file, line))
    {
        // Procura pela linha que contém "write_bytes:"
        if (line.find("write_bytes:") == 0)
        {
            // Extrai o valor de write_bytes da linha.
            std::istringstream iss(line);
            std::string label;
            uint64_t write_bytes;
            // Extrai o valor de write_bytes e o define no objeto metrics.
            iss >> label >> write_bytes;
            metrics.set_write_bytes(write_bytes);
            return;
        }
    }

    // Se não encontrar a linha, define write_bytes como 0.
    metrics.set_write_bytes(0);
}
