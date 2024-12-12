#include "TestClient.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>    // For read()
#include <termios.h>   // For terminal control
#include <sstream>
#include <vector>
#include <stdexcept>
#include <iostream>

// Marshals a glm::mat4 into a string
std::string marshalMat4(const glm::mat4& matrix) {
    std::ostringstream oss;

    // Serialize the matrix row by row
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            oss << matrix[row][col];
            if (row != 3 || col != 3) {
                oss << " "; // Separate elements by a space
            }
        }
    }

    return oss.str();
}

// Unmarshals a string back into a glm::mat4
glm::mat4 unmarshalMat4(const std::string& str) {
     std::cout << str << std::endl;
    if (str == "No other player connected."){
        return glm::mat4(0);
    }
    std::istringstream iss(str);
    std::vector<float> elements;
    float value;

    // Parse the elements from the string
    while (iss >> value) {
        elements.push_back(value);
    }

    // Ensure we have exactly 16 elements for a 4x4 matrix
    if (elements.size() != 16) {
        throw std::invalid_argument("Invalid string format for glm::mat4.");
    }

    // Construct the glm::mat4
    glm::mat4 matrix;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            matrix[row][col] = elements[row * 4 + col];
        }
    }

    return matrix;
}

TestClient::TestClient(const std::string& serverAddress, unsigned short serverPort)
    : client(serverAddress, serverPort), running(false), cameraPos(0.0f),
      cameraLook(0.0f, 0.0f, -1.0f), cameraUp(0.0f, 1.0f, 0.0f), movementSpeed(3.0f),
      deltaTime(0.016f), verticalVelocity(0.0f), gravity(-9.8f), jumpVelocity(5.0f) {}

void TestClient::run() {
    if (!client.VJoin()) {
        std::cerr << "Failed to join the server!" << std::endl;
        return;
    }

    running = true;

    // Start the update loop
    std::thread updateThread(&TestClient::updateLoop, this);

    // Handle input in the main thread
    handleInput();

    running = false;
    if (updateThread.joinable()) {
        updateThread.join();
    }

    client.VDisconnect();
}

// Utility function to set terminal to raw mode
void setRawMode(bool enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);  // Get current terminal settings
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply new settings
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore old settings
    }
}

void TestClient::handleInput() {
    std::cout << "Use W, A, S, D keys to move. Press Q to quit." << std::endl;

    setRawMode(true); // Enable raw mode

    while (running) {
        char inputChar; // Declare inputChar here
        if (read(STDIN_FILENO, &inputChar, 1) > 0) { // Non-blocking read from stdin
            char key = inputChar;

            if (key == 'q' || key == 'Q') {
                running = false;
                break;
            }

            glm::vec3 direction(0.0f);
            if (key == 'w' || key == 'W') direction += cameraLook;
            if (key == 's' || key == 'S') direction -= cameraLook;
            if (key == 'a' || key == 'A') direction -= glm::cross(cameraLook, cameraUp);
            if (key == 'd' || key == 'D') direction += glm::cross(cameraLook, cameraUp);

            if (glm::length(direction) > 0.0f) {
                direction = glm::normalize(direction);
                cameraPos += direction * movementSpeed * deltaTime;
            }

            cameraPos.y += verticalVelocity * deltaTime;
            verticalVelocity += gravity * deltaTime;

            if (cameraPos.y < 1.0f) {
                cameraPos.y = 1.0f;
                verticalVelocity = 0.0f;
            }

            cameraPos = glm::clamp(cameraPos, glm::vec3(-10.0f), glm::vec3(10.0f));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    setRawMode(false); // Restore terminal to normal mode
}

glm::mat4 TestClient::generateViewMatrix() const {
    return glm::lookAt(cameraPos, cameraPos + cameraLook, cameraUp);
}

void TestClient::updateLoop() {
        std::this_thread::sleep_for(std::chrono::seconds(1));

    while (running) {
        glm::mat4 viewMatrix = generateViewMatrix();
        std::string serializedMatrix = marshalMat4(viewMatrix); // Ensure marshalMat4 is implemented
        std::string serverResponse; 
        client.VSync(serializedMatrix, serverResponse);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}
