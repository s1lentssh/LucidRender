#include "VulkanImageView.h"

#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanImage.h>

namespace Lucid::Vulkan
{

VulkanImageView::VulkanImageView(VulkanDevice& device, const VulkanImage& image, vk::Format format)
	: VulkanImageView(device, image.Handle(), format)
{}

VulkanImageView::VulkanImageView(VulkanDevice& device, const vk::Image& image, vk::Format format)
{
	auto imageViewCreateInfo = vk::ImageViewCreateInfo()
		.setImage(image)
		.setViewType(vk::ImageViewType::e2D)
		.setFormat(format)
		.setComponents(vk::ComponentMapping{}) // Identity by default
		.setSubresourceRange(vk::ImageSubresourceRange{}
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseArrayLayer(0)
			.setBaseMipLevel(0)
			.setLayerCount(1)
			.setLevelCount(1));

	mHandle = device.Handle().createImageViewUnique(imageViewCreateInfo);
}

}
