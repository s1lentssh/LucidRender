#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Lucid::Core
{

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 projection;
};

struct MaterialBufferObject
{
    alignas(16) glm::vec4 color = glm::vec4{1.0f};
    alignas(4) float roughness = 0.0f;
    alignas(4) float metalness = 0.0f;
};

} // namespace Lucid::Core
