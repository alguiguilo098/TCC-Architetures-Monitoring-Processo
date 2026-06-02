#include <iostream>
#include <cstdio>
#include <string>

#include "API/ChannelComunication.hpp"
#include "ConfigAgent/ConfigAgent.hpp"

int main()
{
    FILE *pipe = popen(
        "tshark -l -n -i any "
        "-f \"tcp dst port 443\" "
        "-Y \"tls.handshake.extensions_server_name\" "
        "-T fields "
        "-e tls.handshake.extensions_server_name "
        "2>/dev/null",
        "r"
    );

    if (!pipe)
    {
        std::cerr << "Erro ao executar tshark" << std::endl;
        return 1;
    }

    Config configAgent;


    ChannelCommunication channel(
        configAgent.ServerHost,
        configAgent.ServerPort
    );

    char buffer[4096];

    std::cout << "Monitorando URLs..." << std::endl;

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        std::string dominio(buffer);

        // Remove '\n', '\r', espaços e tabs do final
        dominio.erase(
            dominio.find_last_not_of(" \n\r\t") + 1
        );

        if (dominio.empty())
            continue;

        std::cout << "URL acessada: " << dominio << std::endl;

        /*
        ProcessMetricas::UrlAccess urlAccess;
        urlAccess.set_url(dominio);
        channel.sendMessage(urlAccess);
        */
    }

    std::cerr << "Tshark encerrado." << std::endl;

    pclose(pipe);
    return 0;
}