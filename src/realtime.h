#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "fish.h"
#include "terrain/coral.h"
#include "player.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include "utils/sceneparser.h"
#include "fishingrod.h"
#include <thread>

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void rotateCameraRodrigues(glm::vec3& lookVector, glm::vec3& up, float xOffset, float yOffset, float sensitivity = 0.1f);
    void handleServerUpdates();
    void sendUpdatesToServer();

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames
    FishingRod m_fishingRod;
    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;
    std::vector<fish> m_fishVector;
    GLuint m_shader; // Stores id of shader program
    GLuint m_text_overlay;
    GLuint m_vbo;    // Stores id of VBO
    GLuint m_vao;

    // Modeling
    GLuint coral_vao;
    GLuint coral_vbo;
    std::vector<coral*> coral_data;


    glm::mat4 viewMatrix;
    // Timer
    std::thread recvUpdateThread;
    std::thread sendUpdateThread;
    std::atomic<bool> stopThread{false};
    const char* fontPath = "/Users/robertogonzales/Desktop/CS1230/best-cs1230-final-project/src/fonts/timer_font.ttf";
    int secondsRemaining = 30;

    // Gameplay
    float playerNum = 0;
    player Player;

    glm::mat4 m_view;
    RenderData renderData;
};
