#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Lucid::Core
{

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 textureCoordinate;
};

struct PushConstants
{
    glm::vec3 ambientColor;
    alignas(8) float ambientFactor;
    glm::vec3 lightPosition;
    glm::vec3 lightColor;
};

} // namespace Lucid::Core