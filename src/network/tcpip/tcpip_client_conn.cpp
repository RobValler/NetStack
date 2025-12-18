

#include "tcpip_client_conn.h"

#include "message_define.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

void CTCPIP_ClientConn::Stop() {

    close(mClientFD);
}

int CTCPIP_ClientConn::Send(const message::SMessage& msg_data) {

    int body_bytes = 0;

    if(mConnected) {
        auto foo_data(msg_data);
        foo_data.body_size = (int)foo_data.data_array.size();
        (void)write(mClientFD, &foo_data.body_size, sizeof(foo_data.body_size));
        body_bytes = write(mClientFD, &foo_data.data_array[0], foo_data.body_size);
    }
    return body_bytes;
}

int CTCPIP_ClientConn::Receive(message::SMessage& msg_data) {

    auto foo(msg_data);
    auto hdr_size = sizeof(foo.body_size);
    ssize_t hdr_bytes = recv(mClientFD, &foo.body_size, hdr_size, 0);
    if( (hdr_bytes != hdr_size) &&
        (foo.body_size <= 0) ) {
        std::cerr << "Size error" << std::endl;
        return -1;
    }

    //uint16_t msg_size = ntohl(foo.body_size);
    foo.data_array.resize(foo.body_size);
    ssize_t body_bytes = recv(mClientFD, &foo.data_array[0], foo.body_size, 0);

    msg_data = foo;
    return body_bytes;
}

bool CTCPIP_ClientConn::Connected() {

    return true;
};
