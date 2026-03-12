#include <string>
#include "../ProcessMonitoring/ProcessMetricas.pb.h"
#include <sys/socket.h>
#pragma once

class ChannelCommunication
{
private:
    int sock;
    int connected_sock;
    std::string host;
    int port;
    
    void sendTcpJson(const std::string &json, const std::string &host, int port);

public:
    ChannelCommunication(const std::string &host, int port);
    void sendMessage(ProcessMetricas::InstalledProgramList &message);
    void sendMessage(ProcessMetricas::KernelDistro &message);
    void sendMessage(ProcessMetricas::ProcessMetricsList &message);
};