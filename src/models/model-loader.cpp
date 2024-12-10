#include "model-loader.h"
#include "glm/gtc/type_ptr.hpp"
#include "utils/sceneparser.h"
#include <iostream>
#include <numeric>
#include <unistd.h>

modelloader::modelloader() {}


tinygltf::Model modelloader::loadModel(std::string filePath) {

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

    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filePath);

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

// Compute global transforms recursively
void ComputeNodeTransforms(const tinygltf::Model &model, int nodeIndex, const glm::mat4 &parentTransform, std::vector<glm::mat4> &globalTransforms) {
    const tinygltf::Node &node = model.nodes[nodeIndex];
    glm::mat4 localTransform = glm::mat4(1.0f);

    // Apply transformations if available
    if (node.matrix.size() == 16) {
        localTransform = glm::make_mat4(node.matrix.data());
    } else {
        glm::vec3 translation = node.translation.empty() ? glm::vec3(0.0f) : glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
        glm::quat rotation = node.rotation.empty() ? glm::quat(1.0f, 0.0f, 0.0f, 0.0f) : glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
        glm::vec3 scale = node.scale.empty() ? glm::vec3(1.0f) : glm::vec3(node.scale[0], node.scale[1], node.scale[2]);

        localTransform = glm::translate(glm::mat4(1.0f), translation) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);
    }

    globalTransforms[nodeIndex] = parentTransform * localTransform;
    for (int child : node.children) ComputeNodeTransforms(model, child, globalTransforms[nodeIndex], globalTransforms);
}

// Load GLB and return transformed vertices/normals
std::vector<float> modelloader::LoadGLBVerticesNormals(const std::string &filename) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    if (!loader.LoadBinaryFromFile(&model, &err, &warn, filename)) {
        std::cerr << "Error loading GLB: " << filename << "\n" << err << "\n" << warn << std::endl;
        return {};
    }

    // Compute global transforms
    std::vector<glm::mat4> globalTransforms(model.nodes.size(), glm::mat4(1.0f));
    for (int rootNode : model.scenes[model.defaultScene >= 0 ? model.defaultScene : 0].nodes) {
        ComputeNodeTransforms(model, rootNode, glm::mat4(1.0f), globalTransforms);
    }

    // Process vertices and normals
    std::vector<float> finalVertices;
    for (size_t nodeIndex = 0; nodeIndex < model.nodes.size(); ++nodeIndex) {
        const auto &node = model.nodes[nodeIndex];
        if (node.mesh < 0) continue;

        const glm::mat4 &ctm = globalTransforms[nodeIndex];
        const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(ctm)));

        for (const auto &primitive : model.meshes[node.mesh].primitives) {
            if (primitive.attributes.count("POSITION") == 0 || primitive.attributes.count("NORMAL") == 0) continue;

            const auto &posAccessor = model.accessors[primitive.attributes.at("POSITION")];
            const auto &normAccessor = model.accessors[primitive.attributes.at("NORMAL")];
            const auto *posData = model.buffers[model.bufferViews[posAccessor.bufferView].buffer].data.data() + model.bufferViews[posAccessor.bufferView].byteOffset + posAccessor.byteOffset;
            const auto *normData = model.buffers[model.bufferViews[normAccessor.bufferView].buffer].data.data() + model.bufferViews[normAccessor.bufferView].byteOffset + normAccessor.byteOffset;

            const size_t posStride = model.bufferViews[posAccessor.bufferView].byteStride ? model.bufferViews[posAccessor.bufferView].byteStride : 3 * sizeof(float);
            const size_t normStride = model.bufferViews[normAccessor.bufferView].byteStride ? model.bufferViews[normAccessor.bufferView].byteStride : 3 * sizeof(float);

            // Handle indices or sequential access
            std::vector<unsigned int> indices;
            if (primitive.indices >= 0) {
                const auto &idxAccessor = model.accessors[primitive.indices];
                const auto *idxData = model.buffers[model.bufferViews[idxAccessor.bufferView].buffer].data.data() + model.bufferViews[idxAccessor.bufferView].byteOffset + idxAccessor.byteOffset;
                indices.resize(idxAccessor.count);

                for (size_t i = 0; i < idxAccessor.count; ++i) {
                    switch (idxAccessor.componentType) {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:  indices[i] = idxData[i]; break;
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: indices[i] = reinterpret_cast<const uint16_t*>(idxData)[i]; break;
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:   indices[i] = reinterpret_cast<const uint32_t*>(idxData)[i]; break;
                    default: std::cerr << "Unsupported index type." << std::endl; return {};
                    }
                }
            } else {
                indices.resize(posAccessor.count);
                std::iota(indices.begin(), indices.end(), 0);
            }

            // Expand indexed data
            for (unsigned int idx : indices) {
                const float *p = reinterpret_cast<const float*>(posData + idx * posStride);
                const float *n = reinterpret_cast<const float*>(normData + idx * normStride);

                glm::vec4 transformedPos = ctm * glm::vec4(p[0], p[1], p[2], 1.0f);
                glm::vec3 transformedNorm = normalMatrix * glm::vec3(n[0], n[1], n[2]);

                finalVertices.insert(finalVertices.end(), {transformedPos.x, transformedPos.y, transformedPos.z, transformedNorm.x, transformedNorm.y, transformedNorm.z});
            }
        }
    }

    return finalVertices;
}

void modelloader::loadArrayToVBO(GLuint& vbo, GLuint& vao, std::vector<float> data) {
    // Generate and bind VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Correct buffer size and upload data
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);


    // Generate and bind VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Enable and set vertex attribute pointers
    // Attribute 0: Position (3 floats)
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, 0); // Vertex attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, reinterpret_cast<void*>(3*sizeof(GLfloat))); // Normals attribute

    // Unbind VAO and VBO to prevent accidental modifications
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void modelloader::renderModel(GLuint &shader, GLuint &terrain_vao, RenderData renderData, std::vector<float> data) {
    if(!renderData.shapes.empty()) {
        auto shape = renderData.shapes[0];
        auto globalData = renderData.globalData;

        glm::mat4 ctm = glm::mat4(1.0f); // Start with identity matrix

        // Translation: Move the coral to (0, 0, -5)
        ctm = glm::translate(ctm, glm::vec3(0.0f, 0.0f, 0.0f));

        // Rotation: Rotate 45 degrees around Y, 15 degrees around X
        ctm = glm::rotate(ctm, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Y-axis rotation
        ctm = glm::rotate(ctm, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X-axis rotation

        // Scaling: Uniformly scale to half size
        ctm = glm::scale(ctm, glm::vec3(2.0f, 2.0f, 2.0f));


        glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, &ctm[0][0]);
        glUniform4fv(glGetUniformLocation(shader, "shapeColor"), 1, glm::value_ptr(shape.primitive.material.cAmbient * globalData.ka));
        glUniform4fv(glGetUniformLocation(shader, "shapeDiffuse"), 1, glm::value_ptr(shape.primitive.material.cDiffuse * globalData.kd));
        glUniform4fv(glGetUniformLocation(shader, "shapeSpecular"), 1, glm::value_ptr(shape.primitive.material.cSpecular * globalData.ks));
        glUniform1f(glGetUniformLocation(shader, "shininess"), shape.primitive.material.shininess);

        glBindVertexArray(terrain_vao);
        glDrawArrays(GL_TRIANGLES, 0, data.size() / 6);
        glBindVertexArray(0);
    }
}
