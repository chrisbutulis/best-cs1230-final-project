#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#include <string>

// Marshals a glm::mat4 into a string
std::string marshalMat4(const glm::mat4& matrix);

// Unmarshals a string back into a glm::mat4
glm::mat4 unmarshalMat4(const std::string& str);

#endif // UTILS_H
