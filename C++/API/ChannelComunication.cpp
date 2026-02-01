#include "ChannelComunication.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>

void sendTcpJson(const std::string& jsonStr, const std::string& host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket");
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);

    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return;
    }

    // Adiciona quebra de linha no final para o Logstash interpretar cada JSON como um evento
    std::string message = jsonStr + "\n";
    send(sock, message.c_str(), message.size(), 0);

    close(sock);
}


void ChannelCommunication::sendMessage(ProcessMetricas::KernelDistro &message){
    sendTcpJson(message.SerializeAsString(), "0.0.0.0", 9995);
};

void ChannelCommunication::sendMessage(ProcessMetricas::InstalledProgram &message){

}; 