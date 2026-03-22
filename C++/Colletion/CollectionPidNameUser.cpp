#include "Collection.hpp"
#include <array>
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
std::vector<std::string> split(const std::string &str, char separador)
{
    std::vector<std::string> resultado;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, separador))
    {
        resultado.push_back(item);
    }

    return resultado;
}
/**
 * @brief Coleta o nome do processo a partir do arquivo /proc/[pid]/cmdline
 * @param metrics Referência ao objeto ProcessMetrics onde o nome será armazenado
 * @param pid ID do processo cujo nome será coletado
 */
void Collection::get_metrics_name(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    std::ifstream cmd_file("/proc/" + std::to_string(pid) + "/cmdline", std::ios::binary);

    if (cmd_file.is_open())
    {
        std::string name;
        std::getline(cmd_file, name);

        // Se não está vazio
        if (!name.empty())
        {
            std::replace(name.begin(), name.end(), '\0', ' ');
            metrics.set_name(name);
            return;
        }
    }

    // fallback -> comm
    std::ifstream comm_file("/proc/" + std::to_string(pid) + "/comm");

    if (comm_file.is_open())
    {
        std::string comm_name;
        std::getline(comm_file, comm_name);
        metrics.set_name(comm_name);
        return;
    }

    metrics.set_name("unknown");
}
/**
 * @brief Coleta o timestamp atual e o define no objeto ProcessMetrics.
 * @param metrics Referência ao objeto ProcessMetrics onde o timestamp será armazenado.
 * @param pid ID do processo alvo.
 */
void Collection::get_metrics_timestamp(ProcessMetricas::ProcessMetrics &metrics, int pid)
{

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
    localtime_r(&now_c, &tm); // thread-safe no Linux

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");

    metrics.set_timestamp(oss.str());
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
}

/**
 * @brief Coleta o status do processo a partir do arquivo /proc/[pid]/stat
 * @param metrics Referência ao objeto ProcessMetrics onde o status será armazenado
 * @param pid ID do processo cujo status será coletado
 */
void Collection::get_metrics_status(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
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

    // Encontra o final do nome do processo ')'
    auto pos = line.rfind(')');
    if (pos == std::string::npos)
    {
        metrics.set_timestartprocess("");
        return;
    }

    // Pega só a parte depois do nome do processo
    std::istringstream iss(line.substr(pos + 2));

    std::string token;
    long long starttime_jiffies = 0;

    // O starttime agora é o campo 20 (porque pulamos os dois primeiros)
    for (int i = 1; i <= 20; i++)
    {
        iss >> token;
        if (i == 20)
            starttime_jiffies = std::stoll(token);
    }

    long hertz = sysconf(_SC_CLK_TCK);
    double seconds_after_boot = (double)starttime_jiffies / hertz;

    std::ifstream uptime_file("/proc/uptime");
    double uptime = 0.0;
    uptime_file >> uptime;

    std::time_t now = std::time(nullptr);
    std::time_t boot_time = now - (std::time_t)uptime;

    std::time_t process_start_time =
        boot_time + (std::time_t)seconds_after_boot;

    std::tm tm = *std::localtime(&process_start_time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");

    metrics.set_timestartprocess(oss.str());
}