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
        std::string cmd = "ps -p " + std::to_string(pid) + " -o %mem=";

        FILE *pipe = popen(cmd.c_str(), "r");
        if (!pipe)
        {
            metrics.set_memory_percent(0.0f);
            return;
        }

        char buffer[128];
        std::string result;

        if (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result = buffer;
        }

        pclose(pipe);

        if (result.empty())
        {
            metrics.set_memory_percent(0.0f);
            return;
        }

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
    std::ifstream io_file("/proc/" + std::to_string(pid) + "/io");
    if (!io_file.is_open())
    {
        metrics.set_read_bytes(0);
        return;
    }

    std::string line;
    while (std::getline(io_file, line))
    {
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
    std::ifstream io_file("/proc/" + std::to_string(pid) + "/io");
    if (!io_file.is_open())
    {
        metrics.set_write_bytes(0);
        return;
    }

    std::string line;
    while (std::getline(io_file, line))
    {
        if (line.find("write_bytes:") == 0)
        {
            std::istringstream iss(line);
            std::string label;
            uint64_t write_bytes;
            iss >> label >> write_bytes;
            metrics.set_write_bytes(write_bytes);
            return;
        }
    }

    metrics.set_write_bytes(0);
}
