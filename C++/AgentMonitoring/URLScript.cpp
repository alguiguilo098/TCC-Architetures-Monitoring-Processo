#include <iostream>
#include <cstdio>
#include <string>

#include "../API/ChannelComunication.hpp"
#include "../ConfigAgent/ConfigAgent.hpp"
#include <iomanip>
#include "../ProcessMonitoring/ProcessMetricas.pb.h"
#include "../Scripts/Script.hpp"
#include <pwd.h>
#include <unistd.h>
std::string getISO8601Timestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::tm utc_tm{};
    gmtime_r(&time, &utc_tm); // Linux

    std::stringstream ss;
    ss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%SZ");

    return ss.str();
}

#include <cstdio>
#include <string>
#include <sstream>

std::string getLoggedUser()
{
    FILE* pipe = popen(
        "loginctl list-sessions --no-legend | awk '$3 != \"\" {print $3; exit}'",
        "r");

    if (!pipe)
        return "";

    char buffer[256];
    std::string user;

    if (fgets(buffer, sizeof(buffer), pipe))
    {
        user = buffer;

        user.erase(
            user.find_last_not_of(" \n\r\t") + 1);
    }

    pclose(pipe);
    return user;
}

int main()
{
    FILE *pipe = popen(
        "tshark -l -n -i any "
        "-f \"tcp dst port 443\" "
        "-Y \"tls.handshake.extensions_server_name\" "
        "-T fields "
        "-e tls.handshake.extensions_server_name "
        "2>/dev/null",
        "r");

    if (!pipe)
    {
        std::cerr << "Erro ao executar tshark" << std::endl;
        return 1;
    }

    Config configAgent;
    LoadConfig("confagent.conf", configAgent);

    ChannelCommunication channel(
        configAgent.ServerHost,
        configAgent.ServerPort);

    char buffer[4096];
    std::cout << configAgent.ServerHost << std::endl;
    std::cout << configAgent.ServerPort << std::endl;
    std::cout << "Monitorando URLs..." << std::endl;

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        std::string dominio(buffer);

        // Remove '\n', '\r', espaços e tabs do final
        dominio.erase(
            dominio.find_last_not_of(" \n\r\t") + 1);

        if (dominio.empty())
            continue;

        std::cout << "URL acessada: " << dominio << std::endl;

        ProcessMetricas::UrlAccess urlAccess;
        urlAccess.set_url(dominio);

        urlAccess.set_timestamp(getISO8601Timestamp());
        std::string host_ip;
        get_host_ip(host_ip);
        urlAccess.set_hostip(host_ip);
        urlAccess.set_user(getLoggedUser());
        urlAccess.set_laboratory(configAgent.laboratory);
        channel.sendMessage(urlAccess);
    }

    std::cerr << "Tshark encerrado." << std::endl;

    pclose(pipe);
    return 0;
}