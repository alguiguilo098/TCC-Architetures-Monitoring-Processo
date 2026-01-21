#pragma once
#include "../ProcessMonitoring/ProcessMetricas.pb.h"
#include <string>
#include "../ConfigAgent/ConfigAgent.hpp"
#include "../Colletion/Collection.hpp"
#include <chrono>
#include <thread>
#include <semaphore.h>



class AgentMonitoring {
    private:
        Config configAgent;
        Collection* collection;
        sem_t semaphore;
        void monitor_process(int pid, ProcessMetricas::ProcessMetrics &metrics);
        void monitor_kernel_distro(ProcessMetricas::KernelDistro &kernelDistro);
        void monitor_installed_programs(ProcessMetricas::InstalledProgramList &programList);
        void monitor_all_processes();
    public:
        void get_all_pids(std::vector<int> &pids);
        AgentMonitoring(std::string config_path);
        ~AgentMonitoring();
        void load_config(std::string config_path);
        void start_monitoring();

};