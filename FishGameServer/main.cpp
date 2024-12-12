#include "Server.h"
#include <iostream>

int main() {
    unsigned short port = 9000;

    try {
        Server server(port);
        server.run();

        std::cout << "Type 'quit' to stop the server." << std::endl;
        std::string command;
        while (std::getline(std::cin, command)) {
            if (command == "quit") {
                break;
            }
        }

        server.stop();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }

    return 0;
}
