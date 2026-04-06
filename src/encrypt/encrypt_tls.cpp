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
#include <iostream>


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

    // send
    auto foo_data(msg);
    foo_data.body_size = htonl(foo_data.mMsgPayload.size());
    auto header_bytes = SSL_write(mpData->ssl, &foo_data.body_size, sizeof(foo_data.body_size));
    auto bytes_sent = SSL_write(mpData->ssl, foo_data.mMsgPayload.data(), foo_data.mMsgPayload.size());
    return bytes_sent;
}

int EncryptTLS::Receive(message::SMessage& msg) {

    auto foo(msg);
    auto hdr_size = sizeof(foo.body_size);
    ssize_t hdr_bytes = SSL_read(mpData->ssl, &foo.body_size, hdr_size);
    foo.body_size = ntohl(foo.body_size);
    if( (hdr_bytes != hdr_size) &&
        (foo.body_size <= 0) ) {
        std::cerr << "[EncryptTLS] Header size error" << std::endl;
        return -1;
    }

    //uint16_t msg_size = ntohl(foo.body_size);
    foo.mMsgPayload.resize(foo.body_size);
    ssize_t bytes_rec = SSL_read(mpData->ssl, &foo.mMsgPayload[0], foo.body_size);
    if(bytes_rec <= 0) {
        ERR_print_errors_fp(stderr);
        return bytes_rec;
    }

    msg = foo;

    return bytes_rec;
}
