#include "../Scripts/Script.hpp"
#include "../API/ChannelComunication.hpp"
#include "../ProcessMonitoring/ProcessMetricas.pb.h"
int main(int argc, char const *argv[])
{
    ChannelCommunication channel;
    ProcessMetricas::KernelDistro kernelDistro;
    collectionKernelDistro(kernelDistro);
    channel.sendMessage(kernelDistro);
    return 0;
}
