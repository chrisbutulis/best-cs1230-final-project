#include "networksclient.h"
#include <iostream>
#include <cstring>     // For memset
#include <unistd.h>    // For close()
#include <arpa/inet.h> // For socket functions
#include <vector>

// Helper function to construct a message with header and payload
std::vector<char> createMessage(int playerNumber, int protocolNumber, const std::string& payload) {
    int totalLength = 12 + payload.size(); // Header size + payload size

    std::vector<char> message(totalLength);

    // Add length (4 bytes)
    int* lengthPtr = reinterpret_cast<int*>(message.data());
    *lengthPtr = htonl(totalLength);

    // Add player number (4 bytes)
    int* playerPtr = reinterpret_cast<int*>(message.data() + 4);
    *playerPtr = htonl(playerNumber);

    // Add protocol number (4 bytes)
    int* protocolPtr = reinterpret_cast<int*>(message.data() + 8);
    *protocolPtr = htonl(protocolNumber);

    // Add payload
    if (!payload.empty()) {
        std::memcpy(message.data() + 12, payload.data(), payload.size());
    }

    return message;
}

// Helper function to parse the header
void parseHeader(const char* buffer, int& length, int& playerNumber, int& protocolNumber) {
    length = ntohl(*reinterpret_cast<const int*>(buffer));
    playerNumber = ntohl(*reinterpret_cast<const int*>(buffer + 4));
    protocolNumber = ntohl(*reinterpret_cast<const int*>(buffer + 8));
}

NetworkClient::NetworkClient(const std::string& serverAddress, unsigned short serverPort)
    : serverAddress(serverAddress), serverPort(serverPort), clientSocket(-1), connected(false), playerNumber(0) {}

NetworkClient::~NetworkClient() {
    VDisconnect();
}

int NetworkClient::VJoin() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        return 0;
    }

    sockaddr_in serverAddr{};
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        return 0;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection to the server failed");
        return 0;
    }

    connected = true;
    std::cout << "Connected to the server at " << serverAddress << ":" << serverPort << std::endl;

    // Send a join request (Protocol 1)
    auto message = createMessage(0, 1, "");
    if (send(clientSocket, message.data(), message.size(), 0) < 0) {
        perror("Failed to send join request");
        return 0;
    }

    // Receive the player number
    char buffer[12];
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        std::cerr << "Failed to receive player number from server." << std::endl;
        connected = false;
        close(clientSocket);
        clientSocket = -1;
        return 0;
    }

    int length, protocol;
    parseHeader(buffer, length, playerNumber, protocol);
    if (protocol != 1 || playerNumber == 0) {
        std::cerr << "Invalid response or error player number." << std::endl;
        connected = false;
        close(clientSocket);
        clientSocket = -1;
        return 0;
    }

    std::cout << "Assigned player number: " << playerNumber << std::endl;

    return playerNumber;
}

void NetworkClient::VSync(const std::string& updateData, std::string& serverResponse) {
    if (!connected) {
        std::cerr << "Not connected to the server." << std::endl;
        return;
    }

    // Send an update request (Protocol 3)
    auto message = createMessage(playerNumber, 3, updateData);
    if (send(clientSocket, message.data(), message.size(), 0) < 0) {
        perror("Failed to send update data");
        return;
    }

    // Wait for the server's response
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        std::cerr << "Failed to receive data from the server." << std::endl;
        return;
    }

    // Parse the response
    int length, protocol, senderPlayerNumber;
    parseHeader(buffer, length, senderPlayerNumber, protocol);
    if (protocol != 3 || senderPlayerNumber != playerNumber) {
        std::cerr << "Invalid sync response." << std::endl;
        return;
    }

    // Extract the server's response payload
    serverResponse = std::string(buffer + 12, length - 12);
}

void NetworkClient::VDisconnect() {
    if (connected) {
        // Send a disconnect request (Protocol 2)
        auto message = createMessage(playerNumber, 2, "");
        send(clientSocket, message.data(), message.size(), 0);

        connected = false;

        if (clientSocket != -1) {
            close(clientSocket);
            clientSocket = -1;
        }

        if (receiveThread.joinable()) {
            receiveThread.join();
        }

        std::cout << "Disconnected from the server." << std::endl;
    }
}
