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
	VulkanImage(VulkanDevice& device, vk::Image image);
	VulkanImage(VulkanDevice& device, VulkanCommandPool& commandPool, const std::filesystem::path& path);
	
	void Transition(VulkanCommandPool& commandPool, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void Write(VulkanCommandPool& commandPool, const VulkanBuffer& buffer, std::uint32_t width, std::uint32_t height);

	void CreateImageView(vk::Format format);
	const vk::ImageView& GetImageView() const { return mImageView.get(); }

private:
	VulkanDevice& mDevice;

	vk::UniqueDeviceMemory mDeviceMemory;
	vk::UniqueImageView mImageView;

	std::optional<vk::UniqueImage> mUniqueImageHolder;
};

}