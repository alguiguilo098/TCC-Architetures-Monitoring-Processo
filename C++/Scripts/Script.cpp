#include "Script.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <google/protobuf/util/json_util.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
// Descrição: Implementação das funções de script para coleta de métricas, e funções de escrita em arquivos para debug
// Nome: Guilherme Almeida Lopes
// Data: 2025-01-29

void get_host_ip(std::string &ip_address)
{
    //
    struct ifaddrs *ifaddr;
    if (getifaddrs(&ifaddr) == -1)
        return;

    for (struct ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            char ip[INET_ADDRSTRLEN];

            inet_ntop(AF_INET, &(sa->sin_addr), ip, INET_ADDRSTRLEN);

            if (std::strcmp(ip, "127.0.0.1") != 0)
            {
                ip_address = std::string(ip);
                break;
            }
        }
    }

    freeifaddrs(ifaddr);
}

std::string normalizeVersion(const std::string &version)
{
    std::string v = version;

    // Remove epoch (ex: "1:")
    auto colonPos = v.find(':');
    if (colonPos != std::string::npos)
        v = v.substr(colonPos + 1);

    // Corta em '-' ou '+'
    auto dashPos = v.find_first_of("-+");
    if (dashPos != std::string::npos)
        v = v.substr(0, dashPos);

    return v;
}
/// @brief  Coleta informações sobre a distribuição do kernel
/// @param kernelDistro
void collectionKernelDistro(ProcessMetricas::KernelDistro &kernelDistro)
{
    // Host IP
    std::string host;
    get_host_ip(host);
    kernelDistro.set_hostip(host);

    // Timestamp ISO-like
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::ctime(&now_c);
    timestamp.erase(timestamp.find_last_not_of(" \n\r\t") + 1);
    kernelDistro.set_timestamp(timestamp);

    // Kernel version 
    std::ifstream kernel_file("/proc/sys/kernel/osrelease");
    if (kernel_file.is_open()){

        std::string kernel_version;
        std::getline(kernel_file, kernel_version);

        size_t pos = kernel_version.find('-');
        if (pos != std::string::npos)
            kernel_version = kernel_version.substr(0, pos);

        kernelDistro.set_kernel_version(kernel_version);
    }else{
        kernelDistro.set_kernel_version("Unknown");
    }

    // Distro
    std::ifstream os_release_file("/etc/os-release");
    if (os_release_file.is_open())
    {
        std::string line;
        while (std::getline(os_release_file, line))
        {
            if (line.rfind("PRETTY_NAME=", 0) == 0)
            {
                std::string distro_name = line.substr(13, line.length() - 14);
                kernelDistro.set_distro_name(distro_name);
                return;
            }
        }
    }

    kernelDistro.set_distro_name("Unknown");
}

/// @brief Coleta informações sobre os programas instalados
/// @param programList lista de programas instalados
void collectionInstalledPrograms(ProcessMetricas::InstalledProgramList &programList)
{
    
    // hostip
    std::string host;
    get_host_ip(host);
    programList.set_hostip(host);
    
    // Obtém o tempo atual.
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    // Converte para string.
    std::string timestamp = std::ctime(&now_c);

    // Remove espaços em branco e nova linha.
    timestamp.erase(timestamp.find_last_not_of(" \n\r\t") + 1);
    programList.set_timestamp(timestamp);
    // Comando para listar programas instalados (Debian/Ubuntu)
    std::string cmd = "dpkg-query -W -f='${Package} ${Version}\n'";

    // Abre o pipe para ler a saída do comando
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        std::string line(buffer);
        std::istringstream iss(line);
        std::string name, version;
        if (iss >> name >> version)
        {
            ProcessMetricas::InstalledProgram *program = programList.add_programs();
            program->set_name(name);
            program->set_version(normalizeVersion(version));
        }
    }

    pclose(pipe);
}

/// @brief Escreve as métricas de processos coletadas em um arquivo JSON
/// @param metricsList Lista de métricas de processos
/// @param filename Nome do arquivo de saída
void WriteProcessMetricsToFile(
    const ProcessMetricas::ProcessMetricsList &metricsList,
    const std::string &filename)
{
    // Converte a mensagem Protobuf para JSON
    std::string json;
    google::protobuf::util::MessageToJsonString(metricsList, &json);

    // Escreve o JSON em um arquivo
    std::ofstream out(filename, std::ios::out | std::ios::trunc);
    if (!out.is_open())
    {
        throw std::runtime_error("Não foi possível abrir o arquivo: " + filename);
    }

    // Write JSON string to file
    out << json;
    out.close();
}
/// @brief Escreve as informações da distribuição do kernel em um arquivo JSON
/// @param kernelDistro Informações da distribuição do kernel
/// @param filename Nome do arquivo de saída
void WriteKernelDistroToFile(const ProcessMetricas::KernelDistro &kernelDistro, const std::string &filename)
{
    // Converte a mensagem Protobuf para JSON
    std::string json;
    google::protobuf::util::MessageToJsonString(kernelDistro, &json);

    // Escreve o JSON em um arquivo
    std::ofstream out(filename, std::ios::out | std::ios::trunc);
    if (!out.is_open())
    {
        throw std::runtime_error("Não foi possível abrir o arquivo: " + filename);
    }

    // Write JSON string to file
    out << json;
    out.close();
}
/// @brief  Escreve as informações dos programas instalados em um arquivo JSON
/// @param programList Lista de programas instalados
/// @param filename file Nome do arquivo de saída
void WriteInstalledProgramsToFile(const ProcessMetricas::InstalledProgramList &programList, const std::string &filename)
{
    // Converte a mensagem Protobuf para JSON
    std::string json;
    google::protobuf::util::MessageToJsonString(programList, &json);

    // Write JSON string to file
    std::ofstream out(filename, std::ios::out | std::ios::trunc);
    if (!out.is_open())
    {
        throw std::runtime_error("Não foi possível abrir o arquivo: " + filename);
    }

    // Write JSON string to file
    out << json;
    out.close();
}