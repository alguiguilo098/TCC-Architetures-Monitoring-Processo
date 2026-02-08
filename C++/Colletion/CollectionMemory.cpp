#include "Collection.hpp"
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/ioprio.h>
#include <unistd.h>
#include <cerrno>

void Collection::get_mem_statm(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/statm";
    std::ifstream file(path);
    if (!file.is_open()) return;

    long size, resident, shared, text;
    file >> size >> resident >> shared >> text;

    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;

    metrics.set_mem_vms_kb(size * page_size_kb);
    metrics.set_mem_rss_kb(resident * page_size_kb);
    metrics.set_mem_shared_kb(shared * page_size_kb);
    metrics.set_mem_text_kb(text * page_size_kb);
}

void Collection::get_io_nice(ProcessMetricas::ProcessMetrics &metrics, int pid)
{
    int which = IOPRIO_WHO_PROCESS;

    int ioprio = syscall(SYS_ioprio_get, which, pid);

    if (ioprio == -1) {
        // erro
        metrics.set_ionice_value(0);
        return;
    }

    int io_class = IOPRIO_PRIO_CLASS(ioprio);
    int io_data  = IOPRIO_PRIO_DATA(ioprio);

    metrics.set_ionice_value(io_data);

    
}
