#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <array>

namespace Lucid::Vulkan
{

struct VulkanVertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 textureCoordinate;

	static std::array<vk::VertexInputBindingDescription, 1> GetBindingDescriptions();
	static std::array<vk::VertexInputAttributeDescription, 3> GetAttributeDescriptions();
};

struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 projection;
};

}