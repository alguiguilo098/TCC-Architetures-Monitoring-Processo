#include <string>
#include "../ConfigAgent/ConfigAgent.hpp"
#include "../ProcessMonitoring/ProcessMetricas.pb.h"

class Collection {
    public:
        ~Collection();
        Config configAgent;
        Collection(std::string config_path);
        void get_metrics_pid(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_metrics_name(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_metrics_user(ProcessMetricas::ProcessMetrics& metrics, int pid);

        void get_metrics_timestamp(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_metrics_status(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_metrics_create_time(ProcessMetricas::ProcessMetrics& metrics,  int pid);

        void get_metrics_num_threads(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_host_ip(ProcessMetricas::ProcessMetrics& metrics);
        void get_metrics_boottime(ProcessMetricas::ProcessMetrics& metrics);


        void get_metrics_cpu_percent(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_metrics_num_child_processes(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_metrics_memory_percent(ProcessMetricas::ProcessMetrics& metrics, int pid);
        
        void get_metrics_read_bytes(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_metrics_write_bytes(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_metrics_num_fds(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_metrics_nice(ProcessMetricas::ProcessMetrics& metrics, int pid);

        // Coleta informações sobre a distribuição do kernel
        void get_kb_network_sent(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_kb_network_received(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_gid_effective(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_status_sleeping(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_mem_uss(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_mem_rss(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_mem_shared(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_mem_vms(ProcessMetricas::ProcessMetrics& metrics, int pid);
        void get_mem_text(ProcessMetricas::ProcessMetrics& metrics, int pid);
        
};




