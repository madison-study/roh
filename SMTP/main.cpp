#include "TLSClient.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_env_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open .env file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Ignore comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;

        char* equals = strchr(line, '=');
        if (!equals) continue;

        *equals = '\0';
        char* key = line;
        char* value = equals + 1;

        // Remove trailing newline from value
        value[strcspn(value, "\r\n")] = 0;

        // Set environment variable
        setenv(key, value, 1);
    }

    fclose(file);
}

std::string base64_encode(const std::string &input) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    // Do not use newlines to flush buffer
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, input.data(), (int)input.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string encoded(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    return encoded;
}

void send_email() {
    load_env_file("local.env");
    const char* pwd = getenv("GOOGLE");
    std::cout << pwd;
    TLSClient client("google.com", 465);  // use port 587 if doing STARTTLS over plain TCP 

    auto recv = client.recvMessage();
    std::cout << recv;

    client.sendMessage("EHLO example.com\r\n");
    std::cout << client.recvMessage();

    client.sendMessage("AUTH LOGIN\r\n");
    std::cout << client.recvMessage();

    client.sendMessage(base64_encode("overwatch517517@gmail.com") + "\r\n");
    std::cout << client.recvMessage();

    client.sendMessage(base64_encode(pwd) + "\r\n");
    std::cout << client.recvMessage();

    client.sendMessage("MAIL FROM:<overwatch517517@gmail.com>\r\n");
    std::cout << client.recvMessage();

    client.sendMessage("RCPT TO:<roh517@naver.com>\r\n");
    std::cout << client.recvMessage();

    client.sendMessage("DATA\r\n");
    std::cout << client.recvMessage();

    client.sendMessage(
        "Subject: This is Test\r\n"
        "From: overwatch517517@gmail.com\r\n"
        "To: roh517@naver.com\r\n"
        "\r\n"
        "Body of the email.\r\n"
        ".\r\n"
    );
    std::cout << client.recvMessage();

    client.sendMessage("QUIT\r\n");
    std::cout << client.recvMessage();
}

int main() {
    send_email();
    return 0;
}