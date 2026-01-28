#include "../ProcessMonitoring/ProcessMetricas.pb.h"

void collectionKernelDistro(ProcessMetricas::KernelDistro &kernelDistro);
void collectionInstalledPrograms(ProcessMetricas::InstalledProgramList &programList);
void WriteProcessMetricsToFile(const ProcessMetricas::ProcessMetricsList &metricsList, const std::string &filename);
void WriteKernelDistroToFile(const ProcessMetricas::KernelDistro &kernelDistro, const std::string &filename);
void WriteInstalledProgramsToFile(const ProcessMetricas::InstalledProgramList &programList, const std::string &filename);
