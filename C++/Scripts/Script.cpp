#include "Script.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <google/protobuf/util/json_util.h>
// Descrição: Implementação das funções de script para coleta de métricas, e funções de escrita em arquivos para debug
// Nome: Guilherme Almeida Lopes
// Data: 2025-01-29

/// @brief  Coleta informações sobre a distribuição do kernel
/// @param kernelDistro 
void collectionKernelDistro(ProcessMetricas::KernelDistro &kernelDistro){
    // Coleta a versão do kernel
    std::ifstream version_file("/proc/version");
    if (version_file.is_open()) {
        std::string version;
        std::getline(version_file, version);
        kernelDistro.set_kernel_version(version);
    } else {
        kernelDistro.set_kernel_version("Unknown");
    }

    // Coleta o nome da distribuição
    std::ifstream os_release_file("/etc/os-release");
    if (os_release_file.is_open()) {
        std::string line;
        while (std::getline(os_release_file, line)) {
            if (line.find("PRETTY_NAME=") == 0) {
                std::string distro_name = line.substr(13, line.length() - 14); // Remove PRETTY_NAME=" e "
                kernelDistro.set_distro_name(distro_name);
                return;
            }
        }
        kernelDistro.set_distro_name("Unknown");
    } else {
        kernelDistro.set_distro_name("Unknown");
    }
};

/// @brief Coleta informações sobre os programas instalados
/// @param programList lista de programas instalados
void collectionInstalledPrograms(ProcessMetricas::InstalledProgramList &programList){
    // Comando para listar programas instalados (Debian/Ubuntu)
    std::string cmd = "dpkg-query -W -f='${Package} ${Version}\n'";

    // Abre o pipe para ler a saída do comando
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line(buffer);
        std::istringstream iss(line);
        std::string name, version;
        if (iss >> name >> version) {
            ProcessMetricas::InstalledProgram* program = programList.add_programs();
            program->set_name(name);
            program->set_version(version);
        }
    }

    pclose(pipe);
}

/// @brief Escreve as métricas de processos coletadas em um arquivo JSON
/// @param metricsList Lista de métricas de processos
/// @param filename Nome do arquivo de saída
void WriteProcessMetricsToFile(
    const ProcessMetricas::ProcessMetricsList& metricsList,
    const std::string& filename
) {
    // Converte a mensagem Protobuf para JSON
    std::string json;
    google::protobuf::util::MessageToJsonString(metricsList, &json);

    // Escreve o JSON em um arquivo
    std::ofstream out(filename, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
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
    if (!out.is_open()) {
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
    if (!out.is_open()) {
        throw std::runtime_error("Não foi possível abrir o arquivo: " + filename);
    }

    // Write JSON string to file
    out << json;
    out.close();
}