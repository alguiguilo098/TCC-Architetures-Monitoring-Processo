#include "Collection.hpp"
#include <array>
#include <pwd.h>
#include <sstream>
#include <string>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <iomanip>
/**
 * @brief Construtor da classe Collection que carrega a configuração do agente.
 * @param config_path Caminho para o arquivo de configuração.
 */
Collection::Collection(std::string config_path)
{
    LoadConfig(config_path, this->configAgent);
}
Collection::~Collection() {}

/**
 * @brief Coleta o PID do processo e o define no objeto ProcessMetrics.
 * @param metrics Referência ao objeto ProcessMetrics onde o PID será armazenado.
 * @param pid ID do processo alvo.
 */
void Collection::get_metrics_pid(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    metrics.set_pid(pid);
}
/**
 * @brief Coleta o nome do processo a partir do arquivo /proc/[pid]/comm
 * @param metrics Referência ao objeto ProcessMetrics onde o nome será armazenado
 * @param pid ID do processo cujo nome será coletado
 */
void Collection::get_metrics_name(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    std::ifstream status_file("/proc/" + std::to_string(pid) + "/comm");
    if (status_file.is_open())
    {
        // Lê o nome do processo
        std::string name;
        std::getline(status_file, name);
        metrics.set_name(name);
    }
    else
    {
        // Caso não consiga abrir o arquivo, definir nome como "Unknown"
        metrics.set_name("Unknown");
    }
}
/**
 * @brief Coleta o timestamp atual e o define no objeto ProcessMetrics.
 * @param metrics Referência ao objeto ProcessMetrics onde o timestamp será armazenado.
 * @param pid ID do processo alvo.
 */
void Collection::get_metrics_timestamp(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    // Obtém o tempo atual.
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    // Converte para string.
    std::string timestamp = std::ctime(&now_c);
    // Remove espaços em branco e nova linha.
    timestamp.erase(timestamp.find_last_not_of(" \n\r\t") + 1);
    metrics.set_timestamp(timestamp);
}

/**
 * @brief Coleta o nome do usuário do processo a partir do arquivo /proc/[pid]/status
 * @param metrics Referência ao objeto ProcessMetrics onde o nome do usuário será armazenado
 * @param pid ID do processo cujo nome do usuário será coletado
 */
void Collection::get_metrics_user(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    // Abrir o arquivo /proc/<pid>/status
    std::ifstream status_file("/proc/" + std::to_string(pid) + "/status");
    if (!status_file.is_open())
    {
        // Caso não consiga abrir o arquivo, definir usuário como "Unknown"
        metrics.set_user("Unknown");
        return;
    }

    // Ler o arquivo linha por linha para encontrar a linha "Uid:"
    std::string line;
    while (std::getline(status_file, line))
    {
        // Procurar a linha que começa com "Uid:"
        if (line.compare(0, 4, "Uid:") == 0)
        {
            std::istringstream iss(line);
            std::string uid_label;
            int uid_real;

            // Ler o primeiro UID (real) do processo
            iss >> uid_label >> uid_real;

            // Converter UID para nome de usuário
            struct passwd *pw = getpwuid(uid_real);
            if (pw)
            {
                // Definir o nome do usuário no objeto metrics
                metrics.set_user(std::string(pw->pw_name));
            }
            else
            {
                // Caso não consiga converter, definir como "Unknown"
                metrics.set_user("Unknown");
            }
            return;
        }
    }

    // Caso a linha "Uid:" não seja encontrada
    metrics.set_user("");
}

/**
 * @brief Coleta o status do processo a partir do arquivo /proc/[pid]/stat
 * @param metrics Referência ao objeto ProcessMetrics onde o status será armazenado
 * @param pid ID do processo cujo status será coletado
 */
void Collection::get_metrics_status(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    // Abrir o arquivo /proc/<pid>/stat
    std::ifstream stat_file("/proc/" + std::to_string(pid) + "/stat");
    if (!stat_file.is_open())
    {
        // Caso não consiga abrir o arquivo, definir status como "Unknown"
        metrics.set_status("");
        return;
    }

    // Ler o conteúdo do arquivo
    std::string line;
    std::getline(stat_file, line);

    // Analisar a linha para extrair o status (3º campo)
    std::istringstream iss(line);
    std::string token;
    int field = 1;
    std::string status;
    
    while (iss >> token)
    {
        // O campo do status pode estar entre parênteses, então precisamos lidar com isso
        if (field == 3)
        { // 3º campo é o estado
            status = token;
            break;
        }
        field++;

    }
    if (status =="Z")
    {
        // Zombie
        metrics.set_status("Zombie");
        return;
    }else if (status == "S")
    {
        // Sleeping
        metrics.set_status("Sleeping");
        return;
    }else if (status == "R")
    {
        // Running
        metrics.set_status("Running");
        return;
    }
    // Definir o status no objeto metrics
    metrics.set_status(status);
}

/**
 * @brief Coleta o tempo de criação do processo a partir do arquivo /proc/[pid]/stat
 * @param metrics Referência ao objeto ProcessMetrics onde o tempo de criação será armazenado
 * @param pid ID do processo cujo tempo de criação será coletado
 */
void Collection::get_metrics_create_time(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    std::ifstream stat_file("/proc/" + std::to_string(pid) + "/stat");
    if (!stat_file.is_open())
    {
        metrics.set_timestartprocess("");
        return;
    }

    std::string line;
    std::getline(stat_file, line);
    std::istringstream iss(line);

    std::string token;
    int field = 1;
    long long starttime_jiffies = 0;

    while (iss >> token)
    {
        if (field == 22)
        { // starttime
            starttime_jiffies = std::stoll(token);
            break;
        }
        field++;
    }

    // Jiffies por segundo
    long hertz = sysconf(_SC_CLK_TCK);

    // Converte jiffies → segundos desde o boot
    double seconds_after_boot =
        static_cast<double>(starttime_jiffies) / hertz;

    // Lê uptime
    std::ifstream uptime_file("/proc/uptime");
    double uptime = 0.0;
    uptime_file >> uptime;

    // Tempo atual
    std::time_t now = std::time(nullptr);

    // Calcula boot time real
    std::time_t boot_time =
        now - static_cast<std::time_t>(uptime);

    // Tempo real de criação do processo
    std::time_t process_start_time =
        boot_time + static_cast<std::time_t>(seconds_after_boot);

    // Converte para tm
    std::tm tm = *std::localtime(&process_start_time);

    // Formata exatamente:
    // Tue Jan 27 18:27:56 2026
    std::ostringstream oss;
    oss << std::put_time(&tm, "%a %b %d %H:%M:%S %Y");

    metrics.set_timestartprocess(oss.str());
}
