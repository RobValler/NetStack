
#include "tcpip_client.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

CTCPIP_Client::CTCPIP_Client(const SConnectParms& parms)
    : mConnectParms(parms)
{}

int CTCPIP_Client::Start() {

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);

    // Connect to localhost
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    return 0;
}

int CTCPIP_Client::Send(const SNetIF& operater, const std::vector<std::uint8_t>& outgoing_data) {

    // const char* msg = "Hello from client";
    auto bytes = write(sock, outgoing_data.data(), outgoing_data.size());

    return bytes;
}

int CTCPIP_Client::Receive(const SNetIF& operater, std::vector<std::uint8_t>& outgoing_data) {

    // char buffer[1024] = {0};
    outgoing_data.resize(1024);
    auto bytes = read(sock, outgoing_data.data(), outgoing_data.size() -1);
    // std::cout << "Server reply: " << buffer << std::endl;

    return bytes;
}

bool CTCPIP_Client::IsConnected() {

    return true;
}

void CTCPIP_Client::Stop(){

    close(sock);
}
