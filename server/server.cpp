#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cout << "Can't create socket" << std::endl;
        return 1;
    }

    int port = 5555;
    std::string ipAddress = "127.0.0.1";
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    if (bind(sock, (sockaddr*)&hint, sizeof(sockaddr_in)) == -1) {
        std::cout << "Can't bind to IP/port" << std::endl;
        return 1;
    }

    if (listen(sock, SOMAXCONN) == -1) {
        std::cout << "Can't listen" << std::endl;
        return 1;
    }
    std::cout << "Listening on " << ipAddress << ":" << port << std::endl;

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    int newSock = accept(sock, (sockaddr*)&client, &clientSize);
    if (newSock == -1) {
        std::cout << "Problem with accept" << std::endl;
        return 1;
    }

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
        std::cout << host << " connected on port " << service << std::endl;
    } else {
        inet_ntop(AF_INET, &(client.sin_addr), host, NI_MAXHOST);
        std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
    }


    return 0;
}
