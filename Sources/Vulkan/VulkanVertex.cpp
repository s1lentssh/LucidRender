#include "VulkanVertex.h"

namespace Lucid::Vulkan
{

std::array<vk::VertexInputBindingDescription, 1> VulkanVertex::GetBindingDescriptions()
{
	auto description = vk::VertexInputBindingDescription()
		.setBinding(0)
		.setStride(sizeof(VulkanVertex))
		.setInputRate(vk::VertexInputRate::eVertex);

	return { description };
}

std::array<vk::VertexInputAttributeDescription, 3> VulkanVertex::GetAttributeDescriptions()
{
	auto positionDescription = vk::VertexInputAttributeDescription()
		.setBinding(0)
		.setLocation(0)
		.setFormat(vk::Format::eR32G32B32Sfloat)
		.setOffset(offsetof(VulkanVertex, position));

	auto colorDescription = vk::VertexInputAttributeDescription()
		.setBinding(0)
		.setLocation(1)
		.setFormat(vk::Format::eR32G32B32Sfloat)
		.setOffset(offsetof(VulkanVertex, color));

	auto textureCoordinateDescription = vk::VertexInputAttributeDescription()
		.setBinding(0)
		.setLocation(2)
		.setFormat(vk::Format::eR32G32Sfloat)
		.setOffset(offsetof(VulkanVertex, textureCoordinate));

	return { positionDescription, colorDescription, textureCoordinateDescription };
}

}