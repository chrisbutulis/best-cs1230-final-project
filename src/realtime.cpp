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
#include "shapes/Cone.h"
#include "shapes/Cylinder.h"
#include "utils/openglhelper.h"

// ================== Project 5: Lights, Camera

Cone Cone;
Cylinder Cylinder;
Sphere Sphere;
Cube Cube;


Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent),
    m_fishingRod(5.0f, 0.2f)
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
int m_defaultFBO = 2;
float f = 0.f;
GLuint m_fbo_texture;
GLuint m_fbo_renderbuffer;
GLuint m_fbo, m_texture_shader, m_particle_shader;
int m_screen_width;
int m_screen_height;
int m_fbo_width;
int m_fbo_height;
GLuint m_fullscreen_vbo;
GLuint m_fullscreen_vao;
float quadVertices[] = {
    // positions     // texCoords
    -1.0f,  1.0f,    0.0f, 1.0f,
    -1.0f, -1.0f,    0.0f, 0.0f,
    1.0f, -1.0f,    1.0f, 0.0f,
    -1.0f,  1.0f,    0.0f, 1.0f,
    1.0f, -1.0f,    1.0f, 0.0f,
    1.0f,  1.0f,    1.0f, 1.0f
};
float kernel[9] = {
    1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
    2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
    1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
};

    GLuint quadVAO, quadVBO;

    //for kitten
    QImage m_image;
    GLuint m_kitten_texture;

    //for particles
    std::vector<Realtime::Particle> particles;
    unsigned int nr_particles = 500;


void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    GLuint buffers[] = {Cone.vbo, Cylinder.vbo, Sphere.vbo, Cube.vbo};
    GLuint arrays[] = {Cone.vao, Cylinder.vao, Sphere.vao, Cube.vao};

    OpenGLHelper::deleteBuffersAndVAOs(buffers, arrays, 4);

    this->doneCurrent();
}


void makeFBO(){
    // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0); // Bind to texture slot 0
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);

    // Create an empty texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        m_fbo_width,
        m_fbo_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
        );

    // Set min/mag filter interpolation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);


    // Task 20: Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);

    // Allocate storage for depth and stencil buffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);

    // Unbind the renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, 0);


    // Task 18: Generate and bind an FBO
    glGenFramebuffers(1,&m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER,m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // Task 22: Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

void bindKitten() {
    // Task 1: Generate a texture
    glGenTextures(1, &m_kitten_texture);

    // Task 2: Set the active texture slot to texture slot 0
    glActiveTexture(GL_TEXTURE0);

    // Task 3: Bind the texture
    glBindTexture(GL_TEXTURE_2D, m_kitten_texture);

    // Task 4: Define a solid red texture (1x1 pixel)
    unsigned char red_pixel[4] = {255, 0, 0, 255}; // RGBA format (solid red)

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, red_pixel);

    // Task 5: Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Task 6: Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Realtime::initParticles() {
    particles.resize(nr_particles);
    for (auto &particle : particles) {
        particle.Position = glm::vec2(0.0f, 0.0f); // Center of the screen
        particle.Velocity = glm::vec2(
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f, // Random X velocity [-1, 1]
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f  // Random Y velocity [-1, 1]
        );
        particle.Color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f); // Example: orange color
        particle.Life = static_cast<float>(rand()) / RAND_MAX * 5.0f; // Random life span [0, 5]
    }
}

void Realtime::updateParticles(float deltaTime) {
    for (auto &particle : particles) {
        if (particle.Life > 0.0f) {
            // Decrease life
            particle.Life -= deltaTime;
            // Update position based on velocity
            particle.Position += particle.Velocity * deltaTime;
            // Optionally fade out
            particle.Color.a = particle.Life / 5.0f; // Fade alpha over lifetime
        } else {
            // Reset particle
            particle.Position = glm::vec2(0.0f, 0.0f);
            particle.Velocity = glm::vec2(
                (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f,
                (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f
            );
            particle.Life = static_cast<float>(rand()) / RAND_MAX * 5.0f;
        }
    }
}




void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

    // Initializing GL.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Shader setup (DO NOT EDIT)
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/postP.vert", ":/resources/shaders/postP.frag");
    m_particle_shader = ShaderLoader::createShaderProgram(":/resources/shaders/particle.vert", ":/resources/shaders/particle.frag");

    OpenGLHelper::bindVBOVAO(&Cone.vbo, &Cone.vao);
    OpenGLHelper::bindVBOVAO(&Cylinder.vbo, &Cylinder.vao);
    OpenGLHelper::bindVBOVAO(&Sphere.vbo, &Sphere.vao);
    OpenGLHelper::bindVBOVAO(&Cube.vbo, &Cube.vao);
    glUseProgram(m_texture_shader);
    glUniform1i(glGetUniformLocation(m_texture_shader, "s2D"), 0);

    //particles
    bindKitten();
    initParticles();

    std::vector<GLfloat> fullscreen_quad_data = {
        //  POSITIONS          //   UV COORDINATES
        -1.f,  1.f, 0.0f,       0.0f, 1.0f,  // Top-left
        -1.f, -1.f, 0.0f,       0.0f, 0.0f,  // Bottom-left
        1.f, -1.f, 0.0f,       1.0f, 0.0f,  // Bottom-right

        -1.f,  1.f, 0.0f,       0.0f, 1.0f,  // Top-left
        1.f, -1.f, 0.0f,       1.0f, 0.0f,  // Bottom-right
        1.f,  1.f, 0.0f,       1.0f, 1.0f   // Top-right
    };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    // Set the background color to a deep underwater blue
    glClearColor(0.0f, 0.4f, 0.7f, 1.0f);

    // Optional: Enable and configure fog for an underwater effect
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2); // Use exponential fog
    GLfloat fogColor[4] = {0.0f, 0.4f, 0.7f, 1.0f}; // Match background color
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.05f); // Adjust density for desired effect
    glHint(GL_FOG_HINT, GL_NICEST); // Best quality fog


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    makeFBO();
    m_fishVector.push_back(fish(1));
    settings.sceneFilePath = "/home/dhu34/graphics/best-cs1230-final-project/scenefiles/action/required/movement/chess.json";
    sceneChanged();
    settingsChanged();
}

void paintTexture(GLuint texture, bool postP,bool postP2){
    glUseProgram(m_texture_shader);
    glUniform1i(glGetUniformLocation(m_texture_shader, "postP"), postP);
    glUniform1i(glGetUniformLocation(m_texture_shader, "postP2"), postP2);
    glBindVertexArray(m_fullscreen_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

float pressDuration = 0.0f;
bool isThrowing = false;
bool isRetracting = false;
float t = 0;
float cooldownTime = 0.0f;

void Realtime::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render particles
    glUseProgram(m_particle_shader);
    glBindVertexArray(m_fullscreen_vao);
    for (const auto &particle : particles) {
        if (particle.Life > 0.0f) {
            glUniform2f(glGetUniformLocation(m_particle_shader, "particlePosition"), particle.Position.x, particle.Position.y);
            glUniform4f(glGetUniformLocation(m_particle_shader, "particleColor"), particle.Color.r, particle.Color.g, particle.Color.b, particle.Color.a);
            glDrawArrays(GL_POINTS, 0, 1); // Assuming particle shader handles size
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);

//    t+=0.01;
//    glBindFramebuffer(GL_FRAMEBUFFER,m_fbo);
//    glViewport(0, 0, m_fbo_width, m_fbo_height);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    // m_fishingRod.setLineEnd(glm::vec3(4.5,2+2*sin(f*30),2));
//    glUseProgram(m_shader);
//    m_fishingRod.render(m_shader,renderData.globalData);

//    for(int j =0; j<m_fishVector.size();j++){
//        // m_fishVector[j].moveForward();
//        // m_fishVector[j].setRotation(m_fishVector[j].up,glm::sin(t));
//        m_fishVector[j].update(t);
//        if(m_fishingRod.collition(m_fishVector[j].ctm*glm::vec4(0,0,0,1))){
//            m_fishVector[j].changeColor();
//        }
//        m_fishVector[j].render(m_shader,renderData.globalData);
//    }
//    PaintGLHelper::setupMatrices(m_shader, m_view, renderData.cameraData);
//    PaintGLHelper::setupLights(m_shader, renderData.lights);
//    PaintGLHelper::renderShapes(m_shader, renderData.shapes, renderData.globalData);
//    glUseProgram(0);

//    SceneCameraData& camera = renderData.cameraData;
//    glm::vec3 position = glm::vec3(camera.pos);
//    glm::vec3 look = glm::normalize(glm::vec3(camera.look));
//    glm::vec3 up = glm::normalize(glm::vec3(camera.up));
//    m_fishingRod.setBasePosition(position+look+glm::normalize(glm::cross(look,up))*0.5f-up*1.5f);

//    if (m_mouseDown && cooldownTime<0.01) {
//        isRetracting = false;
//        // Increase the press duration while the mouse is held down
//        pressDuration = std::min(pressDuration + 0.05f, 1.0f); // Limit to max 1.0f
//        m_fishingRod.drawFishingRodBack(pressDuration,glm::normalize(glm::cross(look,up)));
//    } else if (pressDuration > 0) {
//        // Start the forward throw on mouse release
//        isThrowing = true;
//        cooldownTime = 3.0f * (1.0f - 0.5f*pressDuration);
//        // pressDuration = 0.0f; // Reset press duration
//    }

//    if (isThrowing) {
//        m_fishingRod.drawFishingRodForward(f,glm::normalize(glm::cross(look,up)),(look*10.f+position));
//        f += (glm::pow(2.f,1.5f*1.5f*pressDuration*pressDuration)/5.f-0.19f); // Progress forward motion
//        if (f > 30.f*pressDuration*pressDuration) {
//            // End the throw after the forward motion is complete
//            isThrowing = false;
//            f = 0.0f;
//            isRetracting = true;
//            pressDuration = 0.0f;
//        }
//    }

//    if (isRetracting){
//        m_fishingRod.retraveLine(glm::normalize(glm::cross(look,up)));
//    }

//    if (cooldownTime > 0.0f) {
//        cooldownTime -= 0.02f; // Decrease cooldown timer (assuming ~60 FPS, adjust accordingly)
//    }

//    //draw particles here?

//    glBindFramebuffer(GL_FRAMEBUFFER,m_defaultFBO);
//    glViewport(0, 0, m_screen_width, m_screen_height);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    paintTexture(m_fbo_texture,settings.perPixelFilter,settings.kernelBasedFilter);
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
    for(int j =0; j<m_fishVector.size();j++){
        m_fishVector[j].setVAOandDataSize(Cube.vao,Cube.generateShape().size());
    }
    m_fishingRod.setVAOandDataSize(Cylinder.vao,Cylinder.generateShape().size());
    OpenGLHelper::updateShapesAndBuffers(renderData, m_view, Cylinder, Cone, Cube, Sphere, settings);
    SceneCameraData& camera = renderData.cameraData;
    glm::vec3 position = glm::vec3(camera.pos);
    glm::vec3 look = glm::normalize(glm::vec3(camera.look));
    glm::vec3 up = glm::normalize(glm::vec3(camera.up));
    m_fishingRod.setBasePosition(position+look+glm::normalize(glm::cross(look,up))*0.5f-up*1.5f);
    update();
}

void Realtime::settingsChanged() {
    OpenGLHelper::updateShapesAndBuffers(renderData, m_view, Cylinder, Cone, Cube, Sphere, settings);
    for(int j =0; j<m_fishVector.size();j++){
        m_fishVector[j].setVAOandDataSize(Cube.vao,Cube.generateShape().size());
    }
    m_fishingRod.setVAOandDataSize(Cylinder.vao,Cylinder.generateShape().size());
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

glm::mat3 rodriguesRotate(const glm::vec3& axis, float angleRadians) {
    glm::vec3 k = glm::normalize(axis);
    float Ux = k.x;
        float Uy = k.y;
        float Uz = k.z;
    float cosTheta = cos(angleRadians);
        float sinTheta = sin(angleRadians);
return glm::mat3 {cosTheta + Ux * Ux * (1 - cosTheta), Ux*Uy * (1 - cosTheta) + Uz * sinTheta, Ux*Uz * (1 - cosTheta) - Uy * sinTheta,
              Ux*Uy * (1 - cosTheta) - Uz * sinTheta, cosTheta + Uy*Uy * (1 - cosTheta), Uy*Uz * (1 - cosTheta) + Ux * sinTheta,
              Ux*Uz * (1 - cosTheta) + Uy * sinTheta, Uy*Uz * (1 - cosTheta) - Ux * sinTheta, cosTheta + Uz*Uz * (1 - cosTheta)};
}



void Realtime::rotateCameraRodrigues(glm::vec3& lookVector, glm::vec3& up, float xOffset, float yOffset, float sensitivity) {

    glm::mat3 Xmat = rodriguesRotate(glm::vec3{0,1,0}, glm::radians(xOffset*0.5f));

    glm::vec3 right = glm::normalize(glm::cross(lookVector, up));
    // m_fishingRod.setRotation(glm::vec3{0,1,0}, glm::radians(xOffset*0.5f));
    glm::mat3 Ymat =  rodriguesRotate(right, glm::radians(yOffset*0.5f));
    // m_fishingRod.setRotation(right, glm::radians(yOffset*0.5f));
    glm::vec3 newLook = Ymat * Xmat * glm::vec3(lookVector);
    glm::vec3 newUp = Ymat * Xmat * glm::vec3(up);
    lookVector = newLook;
}


void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        SceneCameraData& camera = renderData.cameraData;

        // Rotate the camera using mouse displacement
        glm::vec3 look = glm::vec3(camera.look);
        glm::vec3 up = glm::vec3(camera.up);

        rotateCameraRodrigues(look, up, float(-0.5f * deltaX), float(-0.5f * deltaY));

        // Update the camera's look and up vectors
        camera.look = glm::normalize(glm::vec4(look, 0.0f));

    }
    update(); // asks for a PaintGL() call to occur

}


float verticalVelocity = 0.0f;         // Vertical velocity (m/s)
const float gravity = -2.f;          // Gravity acceleration (m/s^2)
const float jumpVelocity = 2.0f;      // Initial jump velocity (m/s)

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f; // Time step in seconds
    m_elapsedTimer.restart();

    // Movement speed
    const float movementSpeed = 3.0f; // Units per second

    // Access the camera's position and look vector
    SceneCameraData& camera = renderData.cameraData;

    glm::vec3 directionVector = {0, 0, 0};
    float deltaPos = deltaTime * movementSpeed;

    // Handle horizontal movement
    if(m_keyMap[Qt::Key_W]) {
        directionVector += glm::vec3(camera.look);
    }
    if(m_keyMap[Qt::Key_S]) {
        directionVector -= glm::vec3(camera.look);
    }
    if(m_keyMap[Qt::Key_A]) {
        directionVector -= glm::cross(glm::vec3(camera.look), glm::vec3(camera.up));
    }
    if(m_keyMap[Qt::Key_D]) {
        directionVector += glm::cross(glm::vec3(camera.look), glm::vec3(camera.up));
    }

    // Normalize direction vector
    if (glm::length(directionVector) > 0.0f) {
        directionVector = glm::normalize(directionVector);
    }

    // Horizontal position update
    camera.pos += glm::vec4(directionVector * deltaPos, 0);

    // Handle jumping
    if (m_keyMap[Qt::Key_Space]) {
        verticalVelocity = jumpVelocity; // Apply initial jump velocity
    }

    // Apply gravity to vertical velocity
    verticalVelocity += gravity * deltaTime;

    // Update vertical position
    camera.pos.y += verticalVelocity * deltaTime;

    // Check for ground collision
    if (camera.pos.y <= 1.0f) { // Assuming ground level is at y = 1.0
        camera.pos.y = 1.0f;    // Reset to ground level
        verticalVelocity = 0.0f; // Stop vertical motion
    }

    updateParticles(deltaTime);

    update(); // Requests a PaintGL() call to occur
}
