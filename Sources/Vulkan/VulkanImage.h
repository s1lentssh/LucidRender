#pragma once

#include <filesystem>
#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanEntity.h>

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanCommandPool;
class VulkanBuffer;

class VulkanImage : public VulkanEntity<vk::UniqueImage>
{
public:
	VulkanImage(VulkanDevice& device, VulkanCommandPool& commandPool, const std::filesystem::path& path);
	
	void Transition(VulkanCommandPool& commandPool, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void Write(VulkanCommandPool& commandPool, const VulkanBuffer& buffer, std::uint32_t width, std::uint32_t height);

private:
	vk::UniqueDeviceMemory mDeviceMemory;
};

}