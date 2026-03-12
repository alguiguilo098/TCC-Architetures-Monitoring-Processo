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
    google::protobuf::util::JsonPrintOptions options;
    options.always_print_primitive_fields = true;
    options.preserve_proto_field_names = true;
    google::protobuf::util::MessageToJsonString(msg, &json, options);
    json.push_back('\n'); // IMPORTANTÍSSIMO: 1 evento por linha
    return json;
}

void ChannelCommunication::sendTcpJson(const std::string& json,
                 const std::string& host,
                 int port)
{
    if (this->sock < 0) {
        perror("socket");
        return;
    }

    if (this->connected_sock < 0) {
        perror("connect");
        close(this->sock);
        return;
    }

    send(this->sock, json.data(), json.size(), 0);
}

ChannelCommunication::ChannelCommunication(const std::string &host, int port)
{
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sock < 0) {
        perror("socket");
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);
    this->host = host;
    this->port = port;

    this->connected_sock = connect(this->sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
}

void ChannelCommunication::sendMessage(ProcessMetricas::InstalledProgramList &message)
{
    sendTcpJson(protobufToJson(message), this->host, this->port);
    
}

void ChannelCommunication::sendMessage(ProcessMetricas::KernelDistro &message)
{
    sendTcpJson(protobufToJson(message), this->host, this->port);
};

void ChannelCommunication::sendMessage(ProcessMetricas::ProcessMetricsList &message)
{
    sendTcpJson(protobufToJson(message), this->host, this->port);
    std::cout << "Connecting to server at " << host << ":" << port << "..." << std::endl;
};