
#include "tcpip_server.h"

#include "message_define.h"

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

    if(server_fd >= 0 ) {
        close(server_fd);
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;   // 0.0.0.0
    addr.sin_port = htons(mConnectParms.portID);

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

    std::cout << "Server listening on port " << std::to_string(mConnectParms.portID) << std::endl;

    client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        return 1;
    }

    std::cout << "Server connected to something..." << std::endl;

    return 0;
}

int CTCPIP_Server::Send(const message::SMessage& msg_data) {

    auto foo_data(msg_data);
    foo_data.body_size = (int)foo_data.data_array.size();
    auto header_bytes = write(client_fd, &foo_data.body_size, sizeof(foo_data.body_size));
    auto body_bytes = write(client_fd, &foo_data.data_array[0], foo_data.body_size);
    return body_bytes;
}

int CTCPIP_Server::Receive(message::SMessage& msg_data) {

    auto foo(msg_data);
    auto hdr_size = sizeof(foo.body_size);
    ssize_t hdr_bytes = recv(client_fd, &foo.body_size, hdr_size, 0);
    if( (hdr_bytes != hdr_size) &&
        (foo.body_size <= 0) ) {
        std::cerr << "Size error" << std::endl;
        return -1;
    }

    //uint16_t msg_size = ntohl(foo.body_size);
    foo.data_array.resize(foo.body_size);
    ssize_t body_bytes = recv(client_fd, &foo.data_array[0], foo.body_size, 0);

    msg_data = foo;
    return body_bytes;
}

bool CTCPIP_Server::IsConnected() {

    return true;
};


void CTCPIP_Server::Stop() {

    close(client_fd);
    close(server_fd);
}
