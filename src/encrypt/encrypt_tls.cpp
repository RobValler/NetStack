/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "encrypt_tls.h"

#include "message_define.h"

#include <openssl/ssl.h>
//#include <openssl/rand.h>
#include <openssl/err.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <cstring>
//#include <iostream>


struct SpImp {

    SSL_CTX* ctx{nullptr};
    SSL* ssl{nullptr};
};


void info_callback(const SSL *ssl, int where, int ret)
{
    const char *state = SSL_state_string_long(ssl);

    if (where & SSL_CB_LOOP) {
        printf("LOOP: %s\n", state);
    } else if (where & SSL_CB_ALERT) {
        printf("ALERT: %s:%s\n",
               SSL_alert_type_string_long(ret),
               SSL_alert_desc_string_long(ret));
    } else if (where & SSL_CB_EXIT) {
        if (ret == 0)
            printf("FAILED: %s\n", state);
        else if (ret < 0)
            printf("ERROR: %s\n", state);
    }
}

EncryptTLS::EncryptTLS()
    : mpData(std::make_unique<SpImp>()) {

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}

EncryptTLS::~EncryptTLS() {

    SSL_shutdown(mpData->ssl);
    SSL_free(mpData->ssl);
    //close(sock);
    SSL_CTX_free(mpData->ctx);
    EVP_cleanup();
}

bool EncryptTLS::Accept(int sock) {

    mpData->ctx = SSL_CTX_new(TLS_server_method());
    if(!mpData->ctx) {
        return false;
    }

    // Load certificate + private key
    if(0 == SSL_CTX_use_certificate_file(mpData->ctx, "cert.pem", SSL_FILETYPE_PEM)) {
        ERR_print_errors_fp(stderr);
        return false;
    }
    if(0 == SSL_CTX_use_PrivateKey_file(mpData->ctx, "key.pem", SSL_FILETYPE_PEM)) {
        ERR_print_errors_fp(stderr);
        return false;
    }

    mpData->ssl = SSL_new(mpData->ctx);
    SSL_set_fd(mpData->ssl, sock);

#if 1
    SSL_CTX_set_info_callback(mpData->ctx, info_callback); // for debugging
#endif

    if (SSL_accept(mpData->ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return false;
    }

    return true;
}

bool EncryptTLS::Connect(int sock) {

    mpData->ctx = SSL_CTX_new(TLS_client_method());
    if(!mpData->ctx) {
        return false;
    }

    // Load certificate + private key
    if(0 == SSL_CTX_use_certificate_file(mpData->ctx, "cert.pem", SSL_FILETYPE_PEM)) {
        ERR_print_errors_fp(stderr);
        return false;
    }
    if(0 == SSL_CTX_use_PrivateKey_file(mpData->ctx, "key.pem", SSL_FILETYPE_PEM)) {
        ERR_print_errors_fp(stderr);
        return false;
    }

    mpData->ssl = SSL_new(mpData->ctx);
    SSL_set_fd(mpData->ssl, sock);

#if 1
    SSL_CTX_set_info_callback(mpData->ctx, info_callback); // for debugging
#endif

    // TLS handshake
    if (SSL_connect(mpData->ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return false;
    }

    return true;
}

int EncryptTLS::Send(const message::SMessage& msg) {

    int total_bytes_sent = 0;
    int expected_bytes_sent = sizeof(std::uint32_t);

    // send the header
    std::uint32_t head_len = htonl(msg.mMsgPayload.size());
    const uint8_t* p_head_len = reinterpret_cast<const uint8_t*>(&head_len);
    while (total_bytes_sent < expected_bytes_sent) {
        int bytes_sent = SSL_write(mpData->ssl, p_head_len + total_bytes_sent, expected_bytes_sent - total_bytes_sent);
        if (bytes_sent <= 0) {
            return bytes_sent;
        }
        total_bytes_sent += bytes_sent;
    }

    // send the payload
    total_bytes_sent = 0;
    expected_bytes_sent = ntohl(head_len);
    while(total_bytes_sent < expected_bytes_sent) {

        int bytes_sent = SSL_write(mpData->ssl, msg.mMsgPayload.data() + total_bytes_sent, expected_bytes_sent - total_bytes_sent);
        if(bytes_sent <= 0) {
            return bytes_sent;
        }
        total_bytes_sent += bytes_sent;
    }

    return total_bytes_sent;
}

int EncryptTLS::Receive(message::SMessage& msg) {

    int total_bytes_received = 0;
    int expected_bytes_sent = sizeof(std::uint32_t);

    // receive the header
    std::uint32_t head_len;
    auto p_head_len = reinterpret_cast<uint8_t*>(&head_len);
    while (total_bytes_received < expected_bytes_sent) {
        int bytes_received = SSL_read(mpData->ssl, p_head_len + total_bytes_received, expected_bytes_sent - total_bytes_received);
        if (bytes_received <= 0) {
            return bytes_received;
        }
        total_bytes_received += bytes_received;
    }

    // receive the payload
    msg.body_size = ntohl(head_len);
    total_bytes_received = 0;
    msg.mMsgPayload.resize(msg.body_size);
    while(total_bytes_received < msg.body_size) {

        int bytes_received = SSL_read(mpData->ssl, &msg.mMsgPayload[0] + total_bytes_received, msg.body_size - total_bytes_received);
        if(bytes_received <= 0) {
            return bytes_received;
        }
        total_bytes_received += bytes_received;
    }

    return total_bytes_received;
}
