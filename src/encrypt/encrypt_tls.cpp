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

    // send the header
    auto foo_data(msg);

    //uint32_t head_len = htonl(msg.mMsgPayload.size());
    uint32_t head_len = msg.mMsgPayload.size();
    std::uint32_t header_bytes = SSL_write(mpData->ssl, &head_len, sizeof(head_len));
    if(header_bytes != sizeof(head_len)) {
        return header_bytes;
    }

    int total = 0;
    while(total < head_len) {

        int n = SSL_write(mpData->ssl, &foo_data.mMsgPayload[0] + total, head_len - total);
        if(n <= 0) {
            return n;
        }
        total += n;
    }

    return total;
}

int EncryptTLS::Receive(message::SMessage& msg) {

    auto foo_data(msg);
    std::uint32_t header_bytes = SSL_read(mpData->ssl, &foo_data.body_size, sizeof(foo_data.body_size));
    if(header_bytes != sizeof(std::uint32_t)) {
        return 0;
    }

    int total = 0;
    int len = foo_data.body_size;
    foo_data.mMsgPayload.resize(len);
    while(total < len) {

        int n = SSL_read(mpData->ssl, &foo_data.mMsgPayload[0] + total, len - total);
        if(n <= 0) {
            return n;
        }
        total += n;
    }

    msg = foo_data;

    return total;
}
