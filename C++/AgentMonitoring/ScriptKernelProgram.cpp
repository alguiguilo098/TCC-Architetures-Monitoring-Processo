#include "../Scripts/Script.hpp"
#include "../API/ChannelComunication.hpp"
#include "../ProcessMonitoring/ProcessMetricas.pb.h"
#include "../ConfigAgent/ConfigAgent.hpp"

int main(int argc, char const *argv[])
{
    Config config;
    LoadConfig(argv[1], config);
    ChannelCommunication channel(config.ServerHost, config.ServerPort);

    ProcessMetricas::InstalledProgramList programList;
    collectionInstalledPrograms(programList);
    channel.sendMessage(programList);
    
    ProcessMetricas::KernelDistro kernelDistro;
    collectionKernelDistro(kernelDistro);
    channel.sendMessage(kernelDistro);
    
    return 0;

}
