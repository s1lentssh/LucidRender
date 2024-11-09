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
    glm::vec2 uv;
    glm::vec4 tangent;
};

struct PushConstants
{
    glm::vec4 ambientColor;
    glm::vec4 lightPosition;
    glm::vec4 lightColor;
};

} // namespace Lucid::Core
