#pragma once

#include <filesystem>
#include <optional>
#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanEntity.h>

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanCommandPool;
class VulkanBuffer;

class VulkanImage : public VulkanEntity<vk::Image>
{
public:
	VulkanImage(
		VulkanDevice& device, 
		std::uint32_t width, 
		std::uint32_t height, 
		vk::Format format,
		vk::ImageTiling tiling, 
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags memoryProperty);

	VulkanImage(VulkanDevice& device, vk::Image image);
	VulkanImage(VulkanDevice& device, VulkanCommandPool& commandPool, const std::filesystem::path& path);
	
	void Transition(VulkanCommandPool& commandPool, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void Write(VulkanCommandPool& commandPool, const VulkanBuffer& buffer, std::uint32_t width, std::uint32_t height);

	void CreateImageView(vk::Format format, vk::ImageAspectFlags aspectFlags);
	const vk::ImageView& GetImageView() const { return mImageView.get(); }

	bool HasStencil(vk::Format format);

protected:
	VulkanDevice& mDevice;

private:
	vk::UniqueDeviceMemory mDeviceMemory;
	vk::UniqueImageView mImageView;

	std::optional<vk::UniqueImage> mUniqueImageHolder;
};

class VulkanDepthImage : public VulkanImage
{
public:
	VulkanDepthImage(VulkanDevice& device, const vk::Extent2D& swapchainExtent);
};

}