
#include "tcpip_server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

CTCPIP_Server::CTCPIP_Server(const SConnectParms& parms)
    : mConnectParms(parms)
{}

int CTCPIP_Server::Start() {

    //mq_unlink(server_fd);

    if(server_fd >= 0 ) {
        close(server_fd);
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;   // 0.0.0.0
    addr.sin_port = htons(8080);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on port 8080...\n";

    client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        return 1;
    }

    std::cout << "Server connected to something...\n";

    return 0;
}

int CTCPIP_Server::Send(const SNetIF& operater, const std::vector<std::uint8_t>& outgoing_data) {

    auto bytes = write(client_fd, outgoing_data.data(), outgoing_data.size());
    return bytes;
}

int CTCPIP_Server::Receive(const SNetIF& operater, std::vector<std::uint8_t>& outgoing_data) {

    ssize_t bytes = read(client_fd, outgoing_data.data(), outgoing_data.size() - 1);
    return bytes;
}

bool CTCPIP_Server::IsConnected() {

    return true;
};


void CTCPIP_Server::Stop() {

    close(client_fd);
    close(server_fd);
}
