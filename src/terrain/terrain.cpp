#include "terrain.h"
#include <iostream>
#include <unistd.h>

terrain::terrain() {}


tinygltf::Model terrain::loadModel(std::string filePath) {

    // Initialize TinyGLTF loader
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;
    std::string warn;

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "Current working directory: " << cwd << std::endl;
    } else {
        perror("getcwd() error");
    }

    bool ret;
    if (filePath.substr(filePath.find_last_of(".") + 1) == "glb") {
        ret = loader.LoadBinaryFromFile(&model, &err, &warn, filePath);
    } else {
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, filePath);
    }

    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to load glTF: " << filePath << std::endl;
    }

    std::cout << "Loaded glTF: " << filePath << std::endl;
    std::cout << "Number of meshes: " << model.meshes.size() << std::endl;
    return model;
}
