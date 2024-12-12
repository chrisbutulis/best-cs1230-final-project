# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(FishGameServer VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(glm)
# Include directories
include_directories(include)

# Add server executable
add_executable(Server main.cpp src/NetworksClient.cpp src/Server.cpp)

# Add client executable
# add_executable(Client src/ClientMain.cpp src/NetworksClient.cpp src/TestClient.cpp)
