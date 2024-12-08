#include "Utils.h"
#include <sstream>
#include <vector>
#include <stdexcept>

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
