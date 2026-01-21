#include "Collection.hpp"
#include <dirent.h>
#include <cstdio>
#include <string>
#include <stdexcept>


void Collection::get_metrics_nice(ProcessMetricas::ProcessMetrics &metrics, int pid){
    std::string stat_path = "/proc/" + std::to_string(pid) + "/stat";
    std::ifstream stat_file(stat_path);
    if (!stat_file.is_open()) {
        metrics.set_nice(0); // Valor padrão se não for possível ler
        return; 
    }
    std::string line;
    std::getline(stat_file, line);
    std::istringstream iss(line);
    std::string token;

    // Pular os primeiros 18 campos
    for (int i = 0; i < 18; ++i) {
        iss >> token;
    }
    int nice_value;
    iss >> nice_value; // O 19º campo é o valor "nice"
    metrics.set_nice(nice_value);

}

void Collection::get_metrics_num_fds(ProcessMetricas::ProcessMetrics &metrics, int pid){

    std::string fd_path = "/proc/" + std::to_string(pid) + "/fd";
    int num_fds = 0;
    DIR* dir = opendir(fd_path.c_str());
    if (dir == nullptr) {
        metrics.set_num_fds(0); // Valor padrão se não for possível ler
        return; 
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_name[0] != '.') { // Ignorar . e ..
            ++num_fds;
        }
    }
    closedir(dir);
    metrics.set_num_fds(num_fds);
}


void Collection::get_metrics_cpu_percent(ProcessMetricas::ProcessMetrics &metrics, int pid) {
    try {
        // Monta o comando ps
        std::string cmd = "ps -p " + std::to_string(pid) + " -o %cpu=";

        // Abre o pipe para ler a saída do comando
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            metrics.set_cpu_percent(0.0f);
            return;
        }

        char buffer[128];
        std::string result;

        // Lê a saída do comando
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result = buffer;
        }

        pclose(pipe);

        // Remove espaços extras e converte para float
        result.erase(0, result.find_first_not_of(" \t\n\r"));
        result.erase(result.find_last_not_of(" \t\n\r") + 1);

        float cpu_percent = 0.0f;
        if (!result.empty()) {
            cpu_percent = std::stof(result);
        }

        metrics.set_cpu_percent(cpu_percent);
    }
    catch (const std::exception& e) {
        // Caso ocorra algum erro, seta 0
        metrics.set_cpu_percent(0.0f);
    }
}


void Collection::get_metrics_num_child_processes(ProcessMetricas::ProcessMetrics &metrics, int pid){
    int num_children = 0;
    std::string task_path = "/proc/" + std::to_string(pid) + "/task";
    DIR* dir = opendir(task_path.c_str());
    if (dir == nullptr) {
        metrics.set_num_child_processes(0); // Valor padrão se não for possível ler
        return; 
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_name[0] != '.') { // Ignorar . e ..
            ++num_children;
        }
    }
    closedir(dir);
    // Subtrai 1 para não contar o processo pai
    metrics.set_num_child_processes(num_children - 1);
}
