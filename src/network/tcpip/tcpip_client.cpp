
#include "tcpip_client.h"

#include "message_define.h"

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
    server_addr.sin_port = htons(mConnectParms.portID);

    // Connect to localhost
    if (inet_pton(AF_INET, mConnectParms.ipAddress.c_str(), &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    std::cout << "Client connected to server" << std::endl;

    return 0;
}

int CTCPIP_Client::Send(const message::SMessage& msg_data) {

    auto foo_data(msg_data);
    foo_data.body_size = (int)foo_data.data_array.size();
    auto header_bytes = write(sock, &foo_data.body_size, sizeof(foo_data.body_size));
    auto body_bytes = write(sock, &foo_data.data_array[0], foo_data.body_size);
    return body_bytes;
}

int CTCPIP_Client::Receive(message::SMessage& msg_data) {

    auto foo(msg_data);
    auto hdr_size = sizeof(foo.body_size);
    ssize_t hdr_bytes = recv(sock, &foo.body_size, hdr_size, 0);
    if( (hdr_bytes != hdr_size) &&
        (foo.body_size <= 0) ) {
        std::cerr << "Size error" << std::endl;
        return -1;
    }

    //uint16_t msg_size = ntohl(foo.body_size);
    foo.data_array.resize(foo.body_size);
    ssize_t body_bytes = recv(sock, &foo.data_array[0], foo.body_size, 0);

    msg_data = foo;
    return body_bytes;
}

bool CTCPIP_Client::IsConnected() {

    return true;
}

void CTCPIP_Client::Stop(){

    close(sock);
}
