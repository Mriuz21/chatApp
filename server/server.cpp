#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

std::mutex mtx;

std :: vector<int> clients;

void handleClient(int newSock)
{     while(true){
        char buffer[4096];
        memset(buffer, 0, 4096);
        int bytesReceived = recv(newSock, buffer, 4096, 0);
        if (bytesReceived == -1) {
            std::cout << "Error in recv()" << std::endl;
            break;
        } else if (bytesReceived == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }
        std::cout << buffer << std::endl;

        // Send the message to all other clients
        mtx.lock();
        for (int clientSock : clients) {
            if (clientSock != newSock) {
                send(clientSock, buffer, strlen(buffer), 0);
            }
        }
        mtx.unlock();
    }
    // Close the socket when the client disconnects
    close(newSock);

}


int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cout << "Can't create socket" << std::endl;
        return 1;
    }
    std::vector<std::thread> threads;
    int port = 5556; 
    std::string ipAddress = "10.0.2.15";
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

    while (true) { // Loop to continuously accept connections
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);
        int newSock = accept(sock, (sockaddr*)&client, &clientSize);
        if (newSock == -1) {
            std::cout << "Problem with accept" << std::endl;
            continue; // Continue to accept other connections
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
        clients.push_back(newSock);
        // Create and start a new thread to handle the client
        threads.push_back(std::thread(handleClient, newSock));
    }

    // Join all threads before exiting
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return 0;
}