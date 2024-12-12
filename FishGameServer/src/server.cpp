#include "Server.h"
#include <iostream>
#include <unistd.h>    // For close()
#include <cstring>     // For memset
#include <arpa/inet.h> // For socket functions

const int TIMER_SECONDS = 30;
// Helper function to construct a message with header and payload
std::vector<char> CreateMessage(int playerNumber, int protocolNumber, const std::string& payload) {
    int totalLength = 12 + payload.size(); // Header size (12 bytes) + payload size

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
void ParseHeader(const char* buffer, int& length, int& playerNumber, int& protocolNumber) {
    length = ntohl(*reinterpret_cast<const int*>(buffer));
    playerNumber = ntohl(*reinterpret_cast<const int*>(buffer + 4));
    protocolNumber = ntohl(*reinterpret_cast<const int*>(buffer + 8));
}

Server::Server(unsigned short port)
    : port(port), serverSocket(-1), running(false), playerCount(0) {}

Server::~Server() {
    stop();
}

void Server::run() {
    running = true;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        return;
    }

    sockaddr_in serverAddr{};
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        return;
    }

    if (listen(serverSocket, 5) < 0) {
        perror("Listen failed");
        return;
    }

    std::cout << "Server running on port " << port << "..." << std::endl;

    std::thread acceptThread(&Server::acceptClients, this);
    acceptThread.detach();
}

void Server::stop() {
    if (running) {
        running = false;

        if (serverSocket != -1) {
            close(serverSocket);
        }

        for (auto& thread : clientThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        std::cout << "Server stopped." << std::endl;
    }
}

void Server::acceptClients() {
    while (running) {
        if (playerCount >= 2) {
            continue; // Maximum of two players
        }

        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            if (running) {
                perror("Accept failed");
            }
            continue;
        }

        int playerNumber = 0;
        {
            // Assign a new player number
            std::lock_guard<std::mutex> lock(playerMutex);
            playerNumber = ++playerCount;
            activePlayers[playerNumber] = clientSocket;
            playerDataMap[playerNumber] = PlayerData{"Default data for player " + std::to_string(playerNumber)};
        }

        std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr)
                  << ":" << ntohs(clientAddr.sin_port)
                  << " assigned player number: " << playerNumber << std::endl;

        // Send the player number to the client (Protocol 1 response)
        auto message = CreateMessage(playerNumber, 1, "");
        send(clientSocket, message.data(), message.size(), 0);

        clientThreads.emplace_back(&Server::handleClient, this, clientSocket, playerNumber);
        if(playerCount == 2) {
            std::thread acceptThread(&Server::startTimer, this);
            acceptThread.detach();
        }
    }
}


void Server::startTimer() {
    if(timerValue != 0) {
        return;
    }

    timerValue = TIMER_SECONDS;
    while(timerValue >= 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        timerValue--;

        for(const auto& player : activePlayers) {
            int playerNumber = player.first;
            int clientSocket = player.second;
            
            auto message = CreateMessage(playerNumber, 4, std::to_string(timerValue));
            send(clientSocket, message.data(), message.size(), 0);
        }
    }
    std::cout << "timer expired" << std::endl;
}

void Server::handleClient(int clientSocket, int playerNumber) {
    char buffer[1024];

    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            std::cout << "Player " << playerNumber << " disconnected" << std::endl;

            // Remove the player from active players
            std::lock_guard<std::mutex> lock(playerMutex);
            activePlayers.erase(playerNumber);
            playerDataMap.erase(playerNumber);
            --playerCount;
            break;
        }

        int length, receivedPlayerNumber, protocol;
        ParseHeader(buffer, length, receivedPlayerNumber, protocol);

        // Validate the player number
        if (receivedPlayerNumber != playerNumber) {
            std::cerr << "Invalid player number from client." << std::endl;
            continue;
        }

        std::string payload(buffer + 12, length - 12); // Extract payload

        if (protocol == 3) { // Unified VSync protocol
            std::string otherPlayerData = "No other player connected.";

            {
                std::lock_guard<std::mutex> lock(playerMutex);

                // Update the player's data
                playerDataMap[playerNumber].data = payload;

                // Fetch the other player's data
                for (const auto& [otherPlayerNumber, otherPlayerDataObj] : playerDataMap) {
                    if (otherPlayerNumber != playerNumber) {
                        otherPlayerData = otherPlayerDataObj.data;
                        break;
                    }
                }
            }

            // Send the other player's data back as a response
            std::vector<char> response = CreateMessage(playerNumber, 3, otherPlayerData);
            send(clientSocket, response.data(), response.size(), 0);
        } else if (protocol == 2) { // Disconnect
            std::cout << "Player " << playerNumber << " requested disconnect." << std::endl;
            close(clientSocket);
            {
                std::lock_guard<std::mutex> lock(playerMutex);
                activePlayers.erase(playerNumber);
                playerDataMap.erase(playerNumber);
                --playerCount;
            }
            return;
        } else {
            std::cerr << "Unknown protocol number: " << protocol << std::endl;
        }
    }

    close(clientSocket);
}


