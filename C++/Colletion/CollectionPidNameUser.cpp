#include "Collection.hpp"

Collection::Collection(std::string config_path) {
    LoadConfig(config_path,this->configAgent);
    std::cout << "Config loaded: \n";
    std::cout << "BufferSize: " << this->configAgent.BufferSize << "\n";
    std::cout << "NumberOfThreads: " << this->configAgent.NumberOfThreads << "\n";
    std::cout << "TruePeriodicScriptHours: " << this->configAgent.TruePeriodicScriptHours << "\n";
    std::cout << "TimeSpleepBetweenReads: " << this->configAgent.TimeSpleepBetweenReads << "\n";

}
Collection::~Collection() {}

void Collection::get_metrics_pid(ProcessMetricas::ProcessMetrics& metrics,int pid) {
    metrics.set_pid(pid);
}
void Collection::get_metrics_name(ProcessMetricas::ProcessMetrics& metrics, int pid) {
    std::ifstream status_file("/proc/" + std::to_string(pid) + "/comm");
    if (status_file.is_open()) {
        std::string name;
        std::getline(status_file, name);
        metrics.set_name(name);
    } else {
        metrics.set_name("Unknown");
    }
}

void Collection::get_metrics_user(ProcessMetricas::ProcessMetrics& metrics, int pid) {
    
}
void Collection::get_metrics_timestamp(ProcessMetricas::ProcessMetrics& metrics, int pid) {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    metrics.set_timestamp(static_cast<int64_t>(now_c));
}

void Collection::get_metrics_status(ProcessMetricas::ProcessMetrics& metrics, int pid) {
    std::ifstream stat_file("/proc/" + std::to_string(pid) + "/stat");
    if (!stat_file.is_open()) {
        metrics.set_status("Unknown");
        return;
    }

    std::string line;
    std::getline(stat_file, line);
    std::istringstream iss(line);
    std::string token;
    int field = 1;
    std::string status;
    while (iss >> token) {
        if (field == 3) { // 3º campo é o estado
            status = token;
            break;
        }
        field++;
    }

    // Traduzir status do Linux para algo legível
    if (status == "R") metrics.set_status("Running");
    else if (status == "S") metrics.set_status("Sleeping");
    else if (status == "D") metrics.set_status("Uninterruptible sleep");
    else if (status == "Z") metrics.set_status("Zombie");
    else if (status == "T") metrics.set_status("Stopped");
    else metrics.set_status("Unknown");
}


