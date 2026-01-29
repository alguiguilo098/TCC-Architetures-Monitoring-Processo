#include "AgentMonitoring.hpp"
#include "../Scripts/Script.hpp"
#include <filesystem>
#include <iostream>
#include <iomanip>
#define DEBUG_AGENT_MONITORING true

// Descrição: Implementação das funções de monitoramento do agente
// Nome: Guilherme Almeida Lopes
// Data: 2025-01-29


/// @brief Monitora um processo específico coletando suas métricas
/// @param pid Identificador do processo a ser monitorado
/// @param metrics Estrutura onde as métricas coletadas serão armazenadas
void AgentMonitoring::monitor_process(int pid, ProcessMetricas::ProcessMetrics &metrics)
{
    // Corretas
    collection->get_metrics_pid(metrics, pid);
    collection->get_metrics_name(metrics, pid);
    collection->get_metrics_user(metrics, pid);
    collection->get_host_ip(metrics);
    collection->get_metrics_timestamp(metrics, pid);
    collection->get_metrics_num_fds(metrics, pid);
    collection->get_metrics_nice(metrics, pid);
    collection->get_metrics_status(metrics, pid);
    collection->get_metrics_boottime(metrics);
    collection->get_metrics_num_threads(metrics, pid);
    collection->get_metrics_num_child_processes(metrics, pid);
    collection->get_metrics_read_bytes(metrics, pid);
    collection->get_metrics_write_bytes(metrics, pid);
    collection->get_metrics_create_time(metrics, pid);
    collection->get_metrics_cpu_percent(metrics, pid);
    collection->get_metrics_memory_percent(metrics, pid);

    

    // Coleta o tempo de atividade do processo

    this->mutexBuffer.lock();
    // Adiciona as métricas coletadas ao buffer de saída
    this->BufferOutput.add_processes()->CopyFrom(metrics);
    this->mutexBuffer.unlock();
}

/// @brief Monitora e coleta informações sobre a distribuição do kernel
/// @param kernelDistro Estrutura onde as informações coletadas serão armazenadas
/// @note Utiliza a função collectionKernelDistro para realizar a coleta

void AgentMonitoring::monitor_kernel_distro(ProcessMetricas::KernelDistro &kernelDistro)
{
    // Coleta informações sobre a distribuição do kernel
    collectionKernelDistro(kernelDistro);
}

/// @brief Monitora e coleta informações sobre os programas instalados
/// @param programList Estrutura onde as informações coletadas serão armazenadas
/// @note Utiliza a função collectionInstalledPrograms para realizar a coleta
void AgentMonitoring::monitor_installed_programs(ProcessMetricas::InstalledProgramList &programList)
{
    // Coleta informações sobre os programas instalados
    collectionInstalledPrograms(programList);
}
/// @brief Monitora todos os processos do sistema coletando suas métricas
/// @note Implementação futura para monitorar todos os processos com múltiplas threads
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
/// @brief Obtém todos os PIDs dos processos em execução no sistema
/// @param pids Vetor onde os PIDs serão armazenados
void AgentMonitoring::get_all_pids(std::vector<int> &pids)
{
    // Itera sobre o diretório /proc para encontrar diretórios que representam PIDs
    for (const auto &entry : fs::directory_iterator("/proc"))
    {
        // Verifica se o nome do diretório é um número (PID)
        if (entry.is_directory())
        {
            // Converte o nome do diretório para inteiro
            std::string dir_name = entry.path().filename().string();
            if (std::all_of(dir_name.begin(), dir_name.end(), ::isdigit))
            {
                // Adiciona o PID à lista
                int pid = std::stoi(dir_name);
                pids.push_back(pid);
            }
        }
    }
}
/// @brief Construtor da classe AgentMonitoring
/// @param config_path Caminho para o arquivo de configuração do agente
/// @note Inicializa a coleção de métricas, listas de programas e distribuição do kernel, além de carregar a configuração
AgentMonitoring::AgentMonitoring(std::string config_path)
{
    this->collection = new Collection(config_path);
    this->programList = ProcessMetricas::InstalledProgramList();
    this->kernelDistro = ProcessMetricas::KernelDistro();
    this->last_monitor_time = std::chrono::steady_clock::now();

    sem_init(&this->semaphoreBuffer, 0, 0);
    load_config(config_path);
}
/// @brief Destrutor da classe AgentMonitoring
/// @note Libera a memória alocada para a coleção de métricas e destrói o semáforo
/// utilizado para sincronização do buffer
AgentMonitoring::~AgentMonitoring()
{
    delete this->collection;
    sem_destroy(&this->semaphoreBuffer);
}

/// @brief Carrega a configuração do agente a partir de um arquivo
/// @param config_path Caminho para o arquivo de configuração
void AgentMonitoring::load_config(std::string config_path)
{
    // Carrega a configuração do agente
    LoadConfig(config_path, this->configAgent);
}

/// @brief Verifica se um intervalo de tempo especificado passou desde o último tempo registrado
/// @param last_time Último tempo registrado
/// @param interval Intervalo de tempo a ser verificado
/// @return true se o intervalo de tempo passou, false caso contrário
bool has_time_passed(
    const std::chrono::steady_clock::time_point &last_time,
    std::chrono::seconds interval)
{
    // Verifica se o intervalo de tempo especificado passou desde o último tempo registrado
    return (std::chrono::steady_clock::now() - last_time) >= interval;
}

void AgentMonitoring::start_monitoring()
{

    while (true)
    {
        // Verifica se é hora de executar os scripts periódicos
        auto now_sys_agent = std::chrono::system_clock::now();
        std::time_t now_time_now = std::chrono::system_clock::to_time_t(now_sys_agent);
        if (has_time_passed(
                this->last_monitor_time,
                std::chrono::hours(this->configAgent.TruePeriodicScriptHours)))
        {
            // Executa os scripts periódicos
            monitor_kernel_distro(kernelDistro);
            // Executa a coleta de programas instalados
            monitor_installed_programs(programList);
            // Atualiza o tempo do último monitoramento
            auto now_sys = std::chrono::system_clock::now();
            std::time_t now_time = std::chrono::system_clock::to_time_t(now_sys);

            std::cout << "[Periodic Script] "
                      << std::put_time(std::localtime(&now_time), "%H:%M:%S")
                      << " Executed periodic scripts."
                      << std::endl;
            // Salvar os dados coletados em arquivos JSON
            WriteKernelDistroToFile(this->kernelDistro, "kernel_distro_data.json");
            WriteInstalledProgramsToFile(this->programList, "installed_programs_data.json");
            // Atualiza o tempo do último monitoramento
            this->last_monitor_time = std::chrono::steady_clock::now();
        }

        if (this->BufferOutput.processes_size() < this->configAgent.BufferSize)
        {
            // Coleta dados de processos
            std::cout << "[Agent Monitoring] "
                      << std::put_time(std::localtime(&now_time_now), "%H:%M:%S")
                      << " First data collection."
                      << this->BufferOutput.processes_size() << std::endl;
            monitor_all_processes();
        }
        else
        {
            // Troca os buffers e escreve os dados coletados em arquivo
            std::cout << "[Agent Monitoring] " << std::put_time(std::localtime(&now_time_now), "%H:%M:%S") << " Subsequent data collection." << std::endl;
            this->BufferInput.Swap(&this->BufferOutput);
            // Write BufferInput to file
            WriteProcessMetricsToFile(this->BufferInput, "process_metrics_data.json");
            this->BufferOutput.Clear();
            sem_post(&this->semaphoreBuffer);
        }

        std::this_thread::sleep_for(std::chrono::seconds(this->configAgent.TimeSpleepBetweenReads));
    }
}

void AgentMonitoring::strart_sending_data_server()
{

    while (true)
    {
        sem_wait(&this->semaphoreBuffer);
        // Send BufferInput to server
        std::cout << "[Agent Monitoring] Sending data to server. Number of processes: " << this->BufferInput.processes_size() << std::endl;
        // After sending, clear the BufferInput
        this->BufferInput.Clear();
    }
}
