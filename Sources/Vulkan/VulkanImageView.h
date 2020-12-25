#pragma once

#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanEntity.h>

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanImage;

class VulkanImageView : public VulkanEntity<vk::UniqueImageView>
{
public:
	VulkanImageView(VulkanDevice& device, const VulkanImage& image, vk::Format format);
	VulkanImageView(VulkanDevice& device, const vk::Image& image, vk::Format format);
};

}