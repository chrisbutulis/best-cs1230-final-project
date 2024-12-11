#ifndef SERVERUPDATE_H
#define SERVERUPDATE_H

#include <string>

struct ServerUpdate {
    int protocol;
    std::string payload;
    int length;
    int senderPlayerNumber;

    ServerUpdate(int protocol, const std::string& payload, int length, int senderPlayerNumber)
        : protocol(protocol), payload(payload), length(length), senderPlayerNumber(senderPlayerNumber) {}
};

#endif // SERVERUPDATE_H
