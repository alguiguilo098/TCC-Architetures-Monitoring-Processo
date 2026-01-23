#pragma once
#include "../ProcessMonitoring/ProcessMetricas.pb.h"
#include <string>
#include "../ConfigAgent/ConfigAgent.hpp"
#include "../Colletion/Collection.hpp"
#include <chrono>
#include <thread>
#include <mutex>
#include <semaphore.h>



class AgentMonitoring {
    private:
        ProcessMetricas::ProcessMetricsList BufferOutput;
        ProcessMetricas::ProcessMetricsList BufferInput;
        Config configAgent;
        Collection* collection;
        std::mutex mutexBuffer;
        std::chrono::steady_clock::time_point last_monitor_time;
        void monitor_process(int pid, ProcessMetricas::ProcessMetrics &metrics);
        void monitor_kernel_distro(ProcessMetricas::KernelDistro &kernelDistro);
        void monitor_installed_programs(ProcessMetricas::InstalledProgramList &programList);
        void monitor_all_processes();
        ProcessMetricas::InstalledProgramList programList;
        ProcessMetricas::KernelDistro kernelDistro;

    public:
        void get_all_pids(std::vector<int> &pids);
        AgentMonitoring(std::string config_path);
        ~AgentMonitoring();
        void load_config(std::string config_path);
        void start_monitoring();

};