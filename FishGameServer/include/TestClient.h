#ifndef TESTCLIENT_H
#define TESTCLIENT_H

#include "NetworksClient.h"

class TestClient {
public:
    TestClient(const std::string& serverAddress, unsigned short serverPort);
    void run();

private:
    NetworkClient client;
    bool running;

    glm::vec3 cameraPos = glm::vec3(0.f,0.f,0.f);
    glm::vec3 cameraLook= glm::vec3(1.f,0.f,0.f);
    glm::vec3 cameraUp= glm::vec3(0.f,1.f,0.f);

    float movementSpeed;
    float deltaTime;
    float verticalVelocity;
    const float gravity;
    const float jumpVelocity;

    void handleInput();
    glm::mat4 generateViewMatrix() const;
    void updateLoop();
};

#endif
