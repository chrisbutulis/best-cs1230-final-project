#include "realtime.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shapes/Cube.h"
#include "shapes/Sphere.h"
#include "utils/shaderloader.h"
#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "shapes/cone.h"
#include "shapes/cylinder.h"
#include "utils/openglhelper.h"
#include "utils/paintglhelper.h"

// ================== Project 5: Lights, Camera

Cone Cone;
Cylinder Cylinder;
Sphere Sphere;
Cube Cube;


Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}



void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    GLuint buffers[] = {Cone.vbo, Cylinder.vbo, Sphere.vbo, Cube.vbo};
    GLuint arrays[] = {Cone.vao, Cylinder.vao, Sphere.vao, Cube.vao};

    OpenGLHelper::deleteBuffersAndVAOs(buffers, arrays, 4);

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Shader setup (DO NOT EDIT)
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    OpenGLHelper::bindVBOVAO(&Cone.vbo, &Cone.vao);
    OpenGLHelper::bindVBOVAO(&Cylinder.vbo, &Cylinder.vao);
    OpenGLHelper::bindVBOVAO(&Sphere.vbo, &Sphere.vao);
    OpenGLHelper::bindVBOVAO(&Cube.vbo, &Cube.vao);
}

void Realtime::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shader);

    PaintGLHelper::setupMatrices(m_shader, m_view, renderData.cameraData);
    PaintGLHelper::setupLights(m_shader, renderData.lights);
    PaintGLHelper::renderShapes(m_shader, renderData.shapes, renderData.globalData);

    glUseProgram(0);
}



void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    settings.aspectRatio = float((w*1.f)/(h*1.f));
    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_view = OpenGLHelper::createPerspectiveProjectionMatrix(
        renderData.cameraData.heightAngle,
        settings.nearPlane,
        settings.farPlane,
        settings.aspectRatio
        );
}

void Realtime::sceneChanged() {
    SceneParser::parse(settings.sceneFilePath, renderData);
    for (auto& shape : renderData.shapes) {
        switch(shape.primitive.type){
        case PrimitiveType::PRIMITIVE_CYLINDER:
            shape.vbo = Cylinder.vbo;
            shape.vao = Cylinder.vao;
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            shape.vbo = Cone.vbo;
            shape.vao = Cone.vao;
            break;
        case PrimitiveType::PRIMITIVE_CUBE:
            shape.vbo = Cube.vbo;
            shape.vao = Cube.vao;
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            shape.vbo = Sphere.vbo;
            shape.vao = Sphere.vao;
            break;
        default:
            break;
        }
    }
    OpenGLHelper::updateShapesAndBuffers(renderData, m_view, Cylinder, Cone, Cube, Sphere, settings);
    update();
}

void Realtime::settingsChanged() {
    OpenGLHelper::updateShapesAndBuffers(renderData, m_view, Cylinder, Cone, Cube, Sphere, settings);
    update();
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
