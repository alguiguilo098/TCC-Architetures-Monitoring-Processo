#include "Script.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>

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
};
