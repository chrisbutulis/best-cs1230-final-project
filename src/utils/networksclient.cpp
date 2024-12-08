#include "networksclient.h"
#include <iostream>
#include <cstring>     // For memset
#include <unistd.h>    // For close()
#include <arpa/inet.h> // For socket functions

NetworkClient::NetworkClient(const std::string& serverAddress, unsigned short serverPort)
    : serverAddress(serverAddress), serverPort(serverPort), clientSocket(-1), connected(false) {}

NetworkClient::~NetworkClient() {
    VDisconnect();
}

bool NetworkClient::VJoin() {
    // Create a socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        return false;
    }

    // Setup the server address structure
    sockaddr_in serverAddr{};
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);

    // Convert and set server address
    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        return false;
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection to the server failed");
        return false;
    }

    connected = true;
    std::cout << "Connected to the server at " << serverAddress << ":" << serverPort << std::endl;

    // Start the receive loop in a separate thread
    receiveThread = std::thread(&NetworkClient::receiveLoop, this);
    return true;
}

std::string NetworkClient::VFetch() {
    if (!connected) {
        std::cerr << "Not connected to the server." << std::endl;
        return "";
    }

    return latestServerData; // Return the most recent server data
}

void NetworkClient::VUpdate(const std::string& data) {
    if (!connected) {
        std::cerr << "Not connected to the server." << std::endl;
        return;
    }

    // Send the data to the server
    if (send(clientSocket, data.c_str(), data.size(), 0) < 0) {
        perror("Failed to send data to the server");
    }
}



void NetworkClient::VDisconnect() {
    if (connected) {
        connected = false;

        // Close the socket
        if (clientSocket != -1) {
            close(clientSocket);
            clientSocket = -1;
        }

        // Join the receive thread
        if (receiveThread.joinable()) {
            receiveThread.join();
        }

        std::cout << "Disconnected from the server." << std::endl;
    }
}

void NetworkClient::receiveLoop() {
    char buffer[1024];

    while (connected) {
        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));

        // Receive data from the server
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            std::cerr << "Server connection lost or closed." << std::endl;
            connected = false;
            break;
        }

        // Store the latest server data
        latestServerData = std::string(buffer, bytesRead);
    }
}
