#include "Collection.hpp"
#include <fstream>
#include <sstream>
#include <string>

#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h> // sysconf

void Collection::get_metrics_memory_percent(ProcessMetricas::ProcessMetrics& metrics, int pid) {
    std::ifstream statm_file("/proc/" + std::to_string(pid) + "/statm");
    if (!statm_file.is_open()) {
        metrics.set_memory_percent(0.0f);
        return;
    }

    long total_program_pages = 0, resident_pages = 0;
    statm_file >> total_program_pages >> resident_pages;

    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024; // tamanho da página em KB
    long rss_kb = resident_pages * page_size_kb;

    // Pegando memória total do sistema
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    long mem_total_kb = 1; // default para evitar divisão por zero
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") == 0) {
            std::istringstream iss(line);
            std::string label;
            iss >> label >> mem_total_kb; // valor já em KB
            break;
        }
    }

    float mem_percent = (static_cast<float>(rss_kb) / mem_total_kb) * 100.0f;
    metrics.set_memory_percent(mem_percent);
}
void Collection::get_metrics_read_bytes(ProcessMetricas::ProcessMetrics& metrics, int pid) {
    std::ifstream io_file("/proc/" + std::to_string(pid) + "/io");
    if (!io_file.is_open()) {
        metrics.set_read_bytes(0);
        return;
    }

    std::string line;
    while (std::getline(io_file, line)) {
        if (line.find("read_bytes:") == 0) {
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
void Collection::get_metrics_write_bytes(ProcessMetricas::ProcessMetrics& metrics, int pid) {
    std::ifstream io_file("/proc/" + std::to_string(pid) + "/io");
    if (!io_file.is_open()) {
        metrics.set_write_bytes(0);
        return;
    }

    std::string line;
    while (std::getline(io_file, line)) {
        if (line.find("write_bytes:") == 0) {
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
