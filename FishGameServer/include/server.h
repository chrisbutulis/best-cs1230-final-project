#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <map>
#include <mutex>
#include <arpa/inet.h> // For socket functions

struct PlayerData {
    std::string data; // Placeholder for player-specific data
};

class Server {
public:
    Server(unsigned short port);
    ~Server();

    void run();    // Start the server
    void stop();   // Stop the server

    void startTimer(); // Start the timer

private:
    void acceptClients(); // Handle incoming connections
    void handleClient(int clientSocket, int playerNumber); // Handle a single client connection

    unsigned short port;                      // Server port
    int serverSocket;                         // Server socket
    std::atomic<bool> running;                // Server running flag
    std::atomic<int> playerCount;             // Count of connected players
    std::vector<std::thread> clientThreads;   // Worker threads for handling clients

    int timerValue = 0;

    std::mutex playerMutex;                   // Mutex for thread-safe access to player data
    std::map<int, int> activePlayers;         // Map of player numbers to client sockets
    std::map<int, PlayerData> playerDataMap;  // Map of player numbers to player data
};

#endif
