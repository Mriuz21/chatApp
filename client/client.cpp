#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread> 

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    std::string name;

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
    std::cout << "Write your name: ";
    std::getline(std::cin, name);
    std::string message;
    while(true)
    {
        std::cout << "Write message: ";
        std::getline(std::cin, message);
        
        // Concatenate name and message
        std::string fullMessage = name + ": " + message;
        
        // Send the full message
        send(sock, fullMessage.c_str(), fullMessage.length(), 0);
        
        // Sleep for 100 milliseconds
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
