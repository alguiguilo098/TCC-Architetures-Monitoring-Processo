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
// Função para obter o timestamp no formato ISO 8601

ProcessMetricas::UrlAccess createUrlAccessMessage(const std::string& url, const std::string& timestamp, const std::string& host_ip, const std::string& user, const std::string& laboratory)
{
    ProcessMetricas::UrlAccess urlAccess;
    urlAccess.set_url(url);
    urlAccess.set_timestamp(timestamp);
    urlAccess.set_hostip(host_ip);
    urlAccess.set_user(user);
    urlAccess.set_laboratory(laboratory);
    return urlAccess;
}
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

// Função para obter o usuário logado
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

    std::cout
        << "Servidor: "
        << configAgent.ServerHost
        << std::endl;

    std::cout
        << "Porta: "
        << configAgent.ServerPort
        << std::endl;
        
    std::cout
        << "Monitorando URLs..."
        << std::endl;

    ChannelCommunication channel(
        configAgent.ServerHost,
        configAgent.ServerPort);

    char buffer[4096];
    
    std::string ultimaUrl = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        std::string dominio(buffer);

        // Remove '\n', '\r', espaços e tabs do final
        dominio.erase(
            dominio.find_last_not_of(" \n\r\t") + 1);

        if (dominio.empty())
            continue;

        if (dominio == ultimaUrl)
            continue;

        ultimaUrl = dominio;
        
        std::cout << "URL acessada: " << dominio << std::endl;

        ProcessMetricas::UrlAccess urlAccess = createUrlAccessMessage(
            dominio,
            getISO8601Timestamp(),
            configAgent.ServerHost,
            getLoggedUser(),
            configAgent.laboratory
        );
        channel.sendMessage(urlAccess);
    }

    std::cerr << "Tshark encerrado." << std::endl;

    pclose(pipe);
    return 0;
}