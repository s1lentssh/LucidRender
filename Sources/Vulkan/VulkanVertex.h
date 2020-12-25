#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <array>

namespace Lucid::Vulkan
{

struct VulkanVertex
{
	glm::vec2 position;
	glm::vec3 color;

	static std::array<vk::VertexInputBindingDescription, 1> GetBindingDescriptions();
	static std::array<vk::VertexInputAttributeDescription, 2> GetAttributeDescriptions();
};

struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 projection;
};

}