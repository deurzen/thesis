#ifndef __FL_APP_HTTP_HH__
#define __FL_APP_HTTP_HH__

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace Http
{
    std::string
    get(
        std::string const& host,
        std::string const& port,
        std::string const& endpoint
    )
    {
        std::string request = "GET http://" + host + ":" + port + "/" + endpoint + " HTTP/1.1\r\n";;
        request += "Host: " + host + "\r\n";
        request += "Accept: */*\r\n";
        request += "Connection: close\r\n";
        request += "\r\n";

        addrinfo hints, *p;
        memset(&hints, 0, sizeof(hints));

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        int addrinfo;
        if ((addrinfo = getaddrinfo(host.c_str(), port.c_str(), &hints, &p)) != 0)
            throw std::runtime_error(gai_strerror(addrinfo));

        if (!p)
            throw std::runtime_error("could not resolve address");

        int sockfd;
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            throw std::runtime_error("could not create socket");

        int connection;
        if ((connection = connect(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
            close(sockfd);
            throw std::runtime_error("could not connect to socket");
        }

        char const *request_raw = request.c_str();
        int bytes = request.size();

        do {
            int sent;
            if ((sent = send(sockfd, request_raw, bytes, 0)) < 0) {
                close(sockfd);
                throw std::runtime_error("error sending request");
            }

            request_raw += sent;
            bytes -= sent;
        } while (bytes > 0);

        std::string reply;
        char buffer[10000];
        bytes = 0;

        while ((bytes = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
            if (bytes == -1) {
                close(sockfd);
                throw std::runtime_error("error receiving bytes");
            }

            reply.append(buffer, bytes);
        }

        close(sockfd);
        freeaddrinfo(p);

        return reply.substr(reply.find("\r\n\r\n") + strlen("\r\n\r\n"));
    }

    std::string
    post(
        std::string const& host,
        std::string const& port,
        std::string const &endpoint,
        std::string const &data
    )
    {
        std::string request = "POST http://" + host + ":" + port + "/" + endpoint + " HTTP/1.1\r\n";;
        request += "Host: " + host + "\r\n";
        request += "Accept: */*\r\n";
        request += "Content-Type: application/x-www-form-urlencoded\r\n";
        request += "Content-Length: " + std::to_string(data.size()) + "\r\n";
        request += "Connection: close\r\n";
        request += "\r\n";
        request += data;

        addrinfo hints, *p;
        memset(&hints, 0, sizeof(hints));

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        int addrinfo;
        if ((addrinfo = getaddrinfo(host.c_str(), port.c_str(), &hints, &p)) != 0)
            throw std::runtime_error(gai_strerror(addrinfo));

        if (!p)
            throw std::runtime_error("could not resolve address");

        int sockfd;
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            throw std::runtime_error("could not create socket");

        int connection;
        if ((connection = connect(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
            close(sockfd);
            throw std::runtime_error("could not connect to socket");
        }

        char const *request_raw = request.c_str();
        int bytes = request.size();

        do {
            int sent;
            if ((sent = send(sockfd, request_raw, bytes, 0)) < 0) {
                close(sockfd);
                throw std::runtime_error("error sending request");
            }

            request_raw += sent;
            bytes -= sent;
        } while (bytes > 0);

        std::string reply;
        char buffer[10000];
        bytes = 0;

        while ((bytes = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
            if (bytes == -1) {
                close(sockfd);
                throw std::runtime_error("error receiving bytes");
            }

            reply.append(buffer, bytes);
        }

        close(sockfd);
        freeaddrinfo(p);

        return reply.substr(reply.find("\r\n\r\n") + strlen("\r\n\r\n"));
    }
}

#endif//__FL_APP_HTTP_HH__
