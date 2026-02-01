#include <string>
#include "../ProcessMonitoring/ProcessMetricas.pb.h"
#pragma once

class ChannelCommunication{
    public:
        
        void sendMessage(ProcessMetricas::KernelDistro &message);
        void sendMessage(ProcessMetricas::InstalledProgram &message); 
};