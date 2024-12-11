cmake_minimum_required(VERSION 3.16)

# Sets project name
project(FishGameClient LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Sets C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Xml)

# Allows you to include files from within those directories, without prefixing their filepaths
include_directories(src)

# Specifies .cpp and .h files to be passed to the compiler
add_executable(${PROJECT_NAME}
    src/main.cpp

    src/realtime.cpp
    src/mainwindow.cpp
    src/settings.cpp
    src/utils/scenefilereader.cpp
    src/utils/sceneparser.cpp

    src/mainwindow.h
    src/realtime.h
    src/settings.h
    src/utils/scenedata.h
    src/utils/scenefilereader.h
    src/utils/sceneparser.h
    src/utils/shaderloader.h
    src/utils/aspectratiowidget/aspectratiowidget.hpp
    src/shapes/Cone.h src/shapes/Cone.cpp
    src/shapes/Cylinder.h src/shapes/Cylinder.cpp
    src/shapes/Sphere.h src/shapes/Sphere.cpp
    src/shapes/Cube.h src/shapes/Cube.cpp
    src/utils/openglhelper.h src/utils/openglhelper.cpp
    src/utils/paintglhelper.h src/utils/paintglhelper.cpp
    src/fishingrod.h src/fishingrod.cpp
    src/fishingline.h src/fishingline.cpp
    src/fish.h src/fish.cpp
    src/particlegenerator.h src/particlegenerator.cpp
    src/models/model-loader.cpp
    src/models/model-loader.h
    src/utils/networksclient.h src/utils/networksclient.cpp
    src/utils/utils.h src/utils/utils.cpp
    src/player.h
    src/player.cpp
)

# GLM: this creates its library and allows you to `#include "glm/..."`
add_subdirectory(glm)

# GLEW: this creates its library and allows you to `#include "GL/glew.h"`
add_library(StaticGLEW STATIC glew/src/glew.c
    src/shapes/Cone.h src/shapes/Cone.cpp)
    lib/tiny_gltf.h
    lib/stb_image.h
    lib/tiny_gltf.h lib/tiny_gltf.cpp
    src/terrain/terraingenerator.h src/terrain/terraingenerator.cpp
    src/terrain/coral.h src/terrain/coral.cpp
    src/terrain/braincoral.h
    src/terrain/tallcoral.h
    src/terrain/braincoral.cpp
    src/terrain/genericcoral.h src/terrain/genericcoral.cpp
)
include_directories(${PROJECT_NAME} PRIVATE glew/include)

# Specifies libraries to be linked (Qt components, glew, etc)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::OpenGL
    Qt::OpenGLWidgets
    Qt::Xml
    StaticGLEW
)

# Specifies other files
qt6_add_resources(${PROJECT_NAME} "Resources"
    PREFIX
        "/"
    FILES
        resources/shaders/default.frag
        resources/shaders/default.vert
        resources/shaders/postP.frag
        resources/shaders/postP.vert
        resources/shaders/particle.frag
        resources/shaders/particle.vert
        resources/shaders/textOverlay.frag
        resources/shaders/textOverlay.vert
        scenes/fish_game.json
        src/models/3d-models/brain_coral.glb
        src/models/3d-models/tall_coral.glb
        src/models/3d-models/generic_coral.glb
        src/models/3d-models/tall_coral.glb
)

# GLEW: this provides support for Windows (including 64-bit)
if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
    glu32
  )
endif()

# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()
# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()
