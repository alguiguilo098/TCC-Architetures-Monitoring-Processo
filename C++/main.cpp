#include "./Colletion/Collection.hpp"
#include "./ProcessMonitoring/ProcessMetricas.pb.h"
#include "./Scripts/Script.hpp"
#define PATH_CONFIG "/home/galmeidalopes/TCC-Architetures-Monitoring-Processo/C++/confagent.conf"
int main(){
    
    ProcessMetricas::ProcessMetrics metrics;
    ProcessMetricas::KernelDistro kernelDistro;
    ProcessMetricas::InstalledProgramList programList;
    collectionKernelDistro(kernelDistro);
    collectionInstalledPrograms(programList);
    Collection* collection= new Collection(PATH_CONFIG);
    collection->get_metrics_pid (metrics,27686);
    collection->get_metrics_name(metrics,27686);
    collection->get_metrics_user(metrics,27686);
    collection->get_metrics_status(metrics,27686);
    collection->get_metrics_timestamp(metrics,27686);
    collection->get_metrics_nice(metrics,27686);
    collection->get_metrics_num_fds(metrics,27686);
    collection->get_metrics_cpu_percent(metrics,27686);

    std::cout << "PID: " << metrics.pid() << "\n";
    std::cout << "Name: " << metrics.name() << "\n";
    std::cout << "User: " << metrics.user() << "\n";
    std::cout << "Status: " << metrics.status() << "\n";
    std::cout << "Timestamp: " << metrics.timestamp() << "\n";
    std::cout << "Nice: " << metrics.nice() << "\n";
    std::cout << "Num FDs: " << metrics.num_fds() << "\n";
    std::cout << "CPU Percent: " << metrics.cpu_percent() << "\n";
    std::cout << "Kernel Version: " << kernelDistro.kernel_version() << "\n";
    std::cout << "Distro Name: " << kernelDistro.distro_name()
                << "\n";
    // std::
    // for (const auto& program : programList.programs()) {
    //     std::cout << "Program Name: " << program.name()
    //               << ", Version: " << program.version() << "\n";
    // }
    delete collection;
    return 0;
}