#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread> 
#include <cstring>

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
    std::string ipAddress = "10.0.2.15";
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

    // Thread for receiving messages
    std::thread receiveThread([&]() {
        char buffer[4096];
        while (true) {
            memset(buffer, 0, 4096);
            int bytesReceived = recv(sock, buffer, 4096, 0);
            if (bytesReceived == -1) {
                std::cout << "Error in recv()" << std::endl;
                break;
            } else if (bytesReceived == 0) {
                std::cout << "Server disconnected" << std::endl;
                break;
            }
            std::cout << buffer << std::endl;
        }
    });

    while(true)
    {
        std::getline(std::cin, message);
        std::string fullMessage = name + ":" + message;
        
        // Send the full message
        send(sock, fullMessage.c_str(), fullMessage.length(), 0);
        
        // Sleep for 100 milliseconds
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Join the receive thread before exiting
    if (receiveThread.joinable()) {
        receiveThread.join();
    }

    return 0;
}
