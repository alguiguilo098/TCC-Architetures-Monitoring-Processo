#include "./Colletion/Collection.hpp"
#include "./ProcessMonitoring/ProcessMetricas.pb.h"
#define PATH_CONFIG "/home/galmeidalopes/TCC-Architetures-Monitoring-Processo/C++/confagent.conf"
int main(){
    
    ProcessMetricas::ProcessMetrics metrics;
    Collection* collection= new Collection(PATH_CONFIG);
    collection->get_metrics_pid (metrics,27686);
    collection->get_metrics_name(metrics,27686);
    collection->get_metrics_user(metrics,27686);
    collection->get_metrics_status(metrics,27686);
    std::cout << "PID: " << metrics.pid() << "\n";
    std::cout << "Name: " << metrics.name() << "\n";
    std::cout << "User: " << metrics.user() << "\n";
    std::cout << "Status: " << metrics.status() << "\n";
    return 0;
}