#include "Collection.hpp"
#include <array>
#include <pwd.h>
#include <sstream>
#include <string>
#include <pwd.h>
#include <sys/types.h>
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
        std::string name;
        std::getline(status_file, name);
        metrics.set_name(name);
    }
    else
    {
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
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::ctime(&now_c);
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
        metrics.set_user("Unknown");
        return;
    }

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
                metrics.set_user(std::string(pw->pw_name));
            }
            else
            {
                metrics.set_user("Unknown");
            }
            return;
        }
    }

    // Caso a linha "Uid:" não seja encontrada
    metrics.set_user("Unknown");
}

/**
 * @brief Coleta o status do processo a partir do arquivo /proc/[pid]/stat
 * @param metrics Referência ao objeto ProcessMetrics onde o status será armazenado
 * @param pid ID do processo cujo status será coletado
 */
void Collection::get_metrics_status(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    std::ifstream stat_file("/proc/" + std::to_string(pid) + "/stat");
    if (!stat_file.is_open())
    {
        metrics.set_status("Unknown");
        return;
    }

    std::string line;
    std::getline(stat_file, line);
    std::istringstream iss(line);
    std::string token;
    int field = 1;
    std::string status;
    while (iss >> token)
    {
        if (field == 3)
        { // 3º campo é o estado
            status = token;
            break;
        }
        field++;
    }

    metrics.set_status(status);
}

void Collection::get_metrics_create_time(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    std::ifstream stat_file("/proc/" + std::to_string(pid) + "/stat");
    if (!stat_file.is_open())
    {
        metrics.set_create_time(0);
        return;
    }

    std::string line;
    std::getline(stat_file, line);
    std::istringstream iss(line);
    std::string token;
    int field = 1;
    long long int starttime = 0;
    while (iss >> token)
    {
        if (field == 22)
        { // 22º campo é o starttime
            starttime = std::stoll(token);
            break;
        }
        field++;
    }

    metrics.set_create_time(starttime);
}
