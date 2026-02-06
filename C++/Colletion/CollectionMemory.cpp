#include "Collection.hpp"
#include <unistd.h>

void Collection::get_mem_statm(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/statm";
    std::ifstream file(path);
    if (!file.is_open()) return;

    long size, resident, shared, text;
    file >> size >> resident >> shared >> text;

    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;

    metrics.set_mem_vms(size * page_size_kb);
    metrics.set_mem_rss(resident * page_size_kb);
    metrics.set_mem_shared(shared * page_size_kb);
    metrics.set_mem_text(text * page_size_kb);
}

void Collection::get_mem_uss(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/smaps";
    std::ifstream file(path);
    if (!file.is_open()) return;
    std::string line;
    long uss_kb = 0;
    while (std::getline(file, line)) {
        if (line.find("Private_Clean:") == 0 || line.find("Private_Dirty:") == 0) {
            std::istringstream iss(line);
            std::string label;
            long value;
            std::string kb;
            iss >> label >> value >> kb;
            uss_kb += value;
        }
    }
    metrics.set_mem_uss(uss_kb);
}
