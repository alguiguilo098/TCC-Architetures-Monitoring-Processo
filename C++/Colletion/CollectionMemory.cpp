#include "Collection.hpp"
#include <unistd.h>

void Collection::get_mem_uss(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/smaps";
    std::ifstream smaps_file(path);
    if (!smaps_file.is_open()) return;

    std::string line;
    float mem_uss = 0.0;

    while (std::getline(smaps_file, line)) {
        if (line.find("Private_Clean:") != std::string::npos ||
            line.find("Private_Dirty:") != std::string::npos) {

            std::istringstream iss(line);
            std::string key, unit;
            float value;

            if (iss >> key >> value >> unit) {
                mem_uss += value; // KB
            }
        }
    }

    metrics.set_mem_uss(mem_uss);
}



void Collection::get_mem_rss(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/statm";
    std::ifstream file(path);
    if (!file.is_open()) return;

    long size, resident;
    file >> size >> resident;

    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    metrics.set_mem_rss(resident * page_size_kb);
}

void Collection::get_mem_shared(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/statm";
    std::ifstream file(path);
    if (!file.is_open()) return;

    long size, resident, shared;
    file >> size >> resident >> shared;

    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    metrics.set_mem_shared(shared * page_size_kb);
}

void Collection::get_mem_vms(ProcessMetricas::ProcessMetrics &metrics, int pid){
    std::string path = "/proc/" + std::to_string(pid) + "/statm";
    std::ifstream file(path);
    if (!file.is_open()) return;

    long size;
    file >> size;

    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    metrics.set_mem_vms(size * page_size_kb);
}


void Collection::get_mem_text(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/statm";
    std::ifstream file(path);
    if (!file.is_open()) return;

    long size, resident, shared, text;
    file >> size >> resident >> shared >> text;

    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    metrics.set_mem_text(text * page_size_kb);
}

