#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <string>
#include <thread>
#include <atomic>

class NetworkClient {
public:
    NetworkClient(const std::string& serverAddress, unsigned short serverPort);
    ~NetworkClient();

    int VJoin();                // Join the server
    void VSync(const std::string& updateData, std::string& serverResponse);
    void VDisconnect();                  // Disconnect from the server

private:
    void receiveLoop();                  // Handles receiving server messages in a separate thread

    std::string serverAddress;           // Server IP address
    unsigned short serverPort;           // Server port
    int clientSocket;                    // Socket descriptor for the client
    std::thread receiveThread;           // Thread for handling incoming data
    std::atomic<bool> connected;         // Connection status
    std::string latestServerData;        // Stores the most recent data from the server
    int playerNumber;
};

#endif
