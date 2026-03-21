#include "./Scripts/Script.hpp"
#include "./API/ChannelComunication.hpp"
#include "./ProcessMonitoring/ProcessMetricas.pb.h"
#include "./ConfigAgent/ConfigAgent.hpp"

#define PATH_CONFIG "./confagent.conf"
int main(int argc, char const *argv[])
{;
    Config config;
    LoadConfig(PATH_CONFIG, config);
    ChannelCommunication channel(config.ServerHost, config.ServerPort);

    ProcessMetricas::InstalledProgramList programList;
    collectionInstalledPrograms(programList);
    channel.sendMessage(programList);
    
    ProcessMetricas::KernelDistro kernelDistro;
    collectionKernelDistro(kernelDistro);
    channel.sendMessage(kernelDistro);
    
    return 0;

}
