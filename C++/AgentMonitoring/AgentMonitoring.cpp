#include "AgentMonitoring.hpp"
#include "../Scripts/Script.hpp"
#include <filesystem>
#include <iostream>
#include <iomanip>
#define DEBUG_AGENT_MONITORING true

void AgentMonitoring::monitor_process(int pid, ProcessMetricas::ProcessMetrics &metrics)
{
    collection->get_metrics_pid(metrics, pid);
    collection->get_metrics_name(metrics, pid);
    collection->get_metrics_user(metrics, pid);
    collection->get_metrics_status(metrics, pid);
    collection->get_metrics_timestamp(metrics, pid);
    collection->get_metrics_nice(metrics, pid);
    collection->get_metrics_num_fds(metrics, pid);
    collection->get_metrics_cpu_percent(metrics, pid);
    collection->get_metrics_num_threads(metrics, pid);
    collection->get_metrics_num_child_processes(metrics, pid);
    collection->get_host_ip(metrics);
    collection->get_metrics_boottime(metrics);
    collection->get_metrics_num_threads(metrics, pid);

    this->mutexBuffer.lock();
    this->BufferOutput.add_processes()->CopyFrom(metrics);
    this->mutexBuffer.unlock();
}

void AgentMonitoring::monitor_kernel_distro(ProcessMetricas::KernelDistro &kernelDistro)
{
    collectionKernelDistro(kernelDistro);
}

void AgentMonitoring::monitor_installed_programs(ProcessMetricas::InstalledProgramList &programList)
{
    collectionInstalledPrograms(programList);
}

void AgentMonitoring::monitor_all_processes()
{
    int num_threads = this->configAgent.NumberOfThreads;
    // Implementação futura para monitorar todos os processos com múltiplas threads
    std::vector<int> pids;
    get_all_pids(pids);

    size_t total_pids = pids.size();
    size_t pids_per_thread = (total_pids + num_threads - 1) / num_threads;
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i)
    {
        size_t start_index = i * pids_per_thread;
        size_t end_index = std::min(start_index + pids_per_thread, total_pids);

        threads.emplace_back([this, &pids, start_index, end_index]()
                             {
            for (size_t j = start_index; j < end_index; ++j)
            {
                ProcessMetricas::ProcessMetrics metrics;
                monitor_process(pids[j], metrics);

                // Aqui você pode armazenar ou processar os dados coletados conforme necessário
            } });
    }

    for (auto &thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}
namespace fs = std::filesystem;

void AgentMonitoring::get_all_pids(std::vector<int> &pids)
{
    for (const auto &entry : fs::directory_iterator("/proc"))
    {
        if (entry.is_directory())
        {
            std::string dir_name = entry.path().filename().string();
            if (std::all_of(dir_name.begin(), dir_name.end(), ::isdigit))
            {
                int pid = std::stoi(dir_name);
                pids.push_back(pid);
            }
        }
    }
}

AgentMonitoring::AgentMonitoring(std::string config_path)
{
    this->collection = new Collection(config_path);
    this->programList = ProcessMetricas::InstalledProgramList();
    this->kernelDistro = ProcessMetricas::KernelDistro();
    this->last_monitor_time = std::chrono::steady_clock::now();
    load_config(config_path);
}

AgentMonitoring::~AgentMonitoring()
{
}

void AgentMonitoring::load_config(std::string config_path)
{
    LoadConfig(config_path, this->configAgent);
}
bool has_time_passed(
    const std::chrono::steady_clock::time_point &last_time,
    std::chrono::seconds interval)
{
    return (std::chrono::steady_clock::now() - last_time) >= interval;
}

void AgentMonitoring::start_monitoring()
{

    while (true)
    {
        auto now_sys_agent = std::chrono::system_clock::now();
        std::time_t now_time_now = std::chrono::system_clock::to_time_t(now_sys_agent);
        if (has_time_passed(
            this->last_monitor_time,
            std::chrono::hours(this->configAgent.TruePeriodicScriptHours)))
            {
                monitor_kernel_distro(kernelDistro);
                monitor_installed_programs(programList);
                auto now_sys = std::chrono::system_clock::now();
                std::time_t now_time = std::chrono::system_clock::to_time_t(now_sys);
                
                std::cout << "[Periodic Script] "
                << std::put_time(std::localtime(&now_time), "%H:%M:%S")
                << " Executed periodic scripts."
                << std::endl;
                
                this->last_monitor_time = std::chrono::steady_clock::now();
            }
            
        monitor_all_processes();
        
        std::cout << "[Agent Monitoring] "<<std::put_time(std::localtime(&now_time_now), "%H:%M:%S")<<" Collected metrics for all processes." << std::endl;
        
        this->mutexBuffer.lock();
        WriteProcessMetricsToFile(this->BufferOutput, "process_metrics_output.json");
        this->mutexBuffer.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(this->configAgent.TimeSpleepBetweenReads));

    }
}
