#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == -1)
    {
        std::cout << "Can't create socket" << std::endl;
        return 1;
    }

    int port = 5555;
    std::string ipAddress = "127.0.0.1";
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(sockaddr_in));
    if(connectRes == -1)
    {
        std::cout << "Can't connect" << std::endl;
        return 1;
    }
    char message[4096];
    while(true)
    {
        std :: cout << "Write message : ";
        std :: cin >> message;
        send(sock, message, strlen(message), 0);
    }
    return 0;
}
