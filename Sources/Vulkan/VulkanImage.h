#pragma once

#include <filesystem>
#include <optional>
#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanEntity.h>
#include <Core/Interfaces.h>

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanCommandPool;
class VulkanBuffer;

/*
	Wrapper around Vulkan image. Could create image from swapchain or from disk.
	Holds vk::UniqueImage for user-created images, since swapchain images are none unique.
*/
class VulkanImage : public VulkanEntity<vk::Image>
{
public:
	static std::unique_ptr<VulkanImage> CreateDepthImage(
		VulkanDevice& device, 
		const vk::Extent2D& swapchainExtent, 
		vk::Format format, 
		vk::ImageAspectFlags aspectFlags);

	static std::unique_ptr<VulkanImage> CreateImage(
		VulkanDevice& device,
		vk::Image image,
		vk::Format format,
		vk::ImageAspectFlags aspectFlags);

	static std::unique_ptr<VulkanImage> CreateImageFromResource(
		VulkanDevice& device,
		VulkanCommandPool& commandPool,
		const std::filesystem::path& path,
		vk::Format format,
		vk::ImageAspectFlags aspectFlags);
	
	void Transition(VulkanCommandPool& commandPool, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void Write(VulkanCommandPool& commandPool, const VulkanBuffer& buffer, const Core::Vector2d<std::uint32_t>& size);

	[[nodiscard]] const vk::ImageView& GetImageView() const;
	[[nodiscard]] bool HasStencil(vk::Format format) const;

private:
	VulkanImage(
		VulkanDevice& device,
		std::uint32_t width,
		std::uint32_t height,
		vk::Format format,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags memoryProperty);

	VulkanImage(
		VulkanDevice& device,
		vk::Image image);

	VulkanImage(
		VulkanDevice& device,
		VulkanCommandPool& commandPool,
		const std::filesystem::path& path);

	void CreateImageView(vk::Format format, vk::ImageAspectFlags aspectFlags);
	
	VulkanDevice& mDevice;
	vk::UniqueDeviceMemory mDeviceMemory;
	vk::UniqueImageView mImageView;
	std::optional<vk::UniqueImage> mUniqueImageHolder;
};

}