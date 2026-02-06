#include "ChannelComunication.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <google/protobuf/util/json_util.h>

std::string protobufToJson(const google::protobuf::Message& msg)
{
    std::string json;
    google::protobuf::util::MessageToJsonString(msg, &json);
    json.push_back('\n'); // IMPORTANTÍSSIMO: 1 evento por linha
    return json;
}

void sendTcpJson(const std::string& json,
                 const std::string& host,
                 int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return;
    }

    send(sock, json.data(), json.size(), 0);
    close(sock);
}



void ChannelCommunication::sendMessage(ProcessMetricas::InstalledProgramList &message)
{
    sendTcpJson(protobufToJson(message), "0.0.0.0", 9995);
}

void ChannelCommunication::sendMessage(ProcessMetricas::KernelDistro &message)
{
    sendTcpJson(protobufToJson(message), "0.0.0.0", 9995);
};

void ChannelCommunication::sendMessage(ProcessMetricas::ProcessMetricsList &message)
{
    sendTcpJson(protobufToJson(message), "0.0.0.0", 9995);
};