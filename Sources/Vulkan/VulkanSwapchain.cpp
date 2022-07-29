#include "VulkanSwapchain.h"

#include <Vulkan/VulkanRenderPass.h>
#include <Vulkan/VulkanBuffer.h>
#include <Utils/Logger.hpp>

namespace Lucid::Vulkan
{

VulkanSwapchain::VulkanSwapchain(VulkanDevice& device, const VulkanSurface& surface, const Core::Vector2d<std::uint32_t>& size)
	: mWindowSize(size)
	, mDevice(device)
	, mSurface(surface)
{
	VulkanDevice::SwapchainDetails details = mDevice.GetSwapchainDetails(mSurface);

	vk::SurfaceFormatKHR surfaceFormat = SelectSurfaceFormat(details.formats);
	vk::PresentModeKHR presentMode = SelectPresentMode(details.presentModes);
	vk::Extent2D extent = SelectSwapExtent(details.capabilities);

	mFormat = surfaceFormat.format;
	mExtent = extent;

	std::uint32_t imageCount = details.capabilities.minImageCount + 1;
	bool shouldStickToMaximumImageCount = details.capabilities.maxImageCount > 0;

	if (shouldStickToMaximumImageCount && imageCount > details.capabilities.maxImageCount)
	{
		imageCount = details.capabilities.maxImageCount;
	}

	auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR()
		.setSurface(mSurface.Handle().get())
		.setMinImageCount(imageCount)
		.setImageFormat(surfaceFormat.format)
		.setImageColorSpace(surfaceFormat.colorSpace)
		.setImageExtent(extent)
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setPreTransform(details.capabilities.currentTransform)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setPresentMode(presentMode)
		.setClipped(true);

	std::uint32_t queueFamilies[] = {
		mDevice.FindGraphicsQueueFamily().value(),
		mDevice.FindPresentQueueFamily(mSurface).value()
	};

	if (queueFamilies[0] != queueFamilies[1])
	{
		swapchainCreateInfo
			.setQueueFamilyIndexCount(2)
			.setPQueueFamilyIndices(queueFamilies)
			.setImageSharingMode(vk::SharingMode::eConcurrent);
	}
	else
	{
		swapchainCreateInfo
			.setImageSharingMode(vk::SharingMode::eExclusive);
	}

	mHandle = mDevice.Handle()->createSwapchainKHRUnique(swapchainCreateInfo);
	LoggerInfo << "Swapchain created";

	auto swapchainImages = mDevice.Handle()->getSwapchainImagesKHR(Handle().get());
	for (const auto& image : swapchainImages)
	{
		mImages.push_back(VulkanImage::FromSwapchain(mDevice, image, mFormat, vk::ImageAspectFlagBits::eColor));
	}
}

vk::ResultValue<std::uint32_t> VulkanSwapchain::AcquireNextImage(const vk::UniqueSemaphore& semaphore)
{
	return mDevice.Handle()->acquireNextImageKHR(
		Handle().get(), 
		std::numeric_limits<std::uint64_t>::max(), 
		semaphore.get(), {});
}

const std::vector<vk::UniqueFramebuffer>& VulkanSwapchain::GetFramebuffers() const noexcept 
{ 
	return mFramebuffers; 
}

vk::SurfaceFormatKHR VulkanSwapchain::SelectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const noexcept
{
	assert(!availableFormats.empty());

	for (const auto& availableFormat : availableFormats)
	{
		bool niceFormat = vk::Format::eR8G8B8A8Srgb == availableFormat.format;
		bool niceColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear == availableFormat.colorSpace;

		if (niceFormat && niceColorSpace)
		{
			LoggerInfo << "Selected nice colorspace and format";
			return availableFormat;
		}
	}

	LoggerInfo << "Selected available colorspace and format";

	if (availableFormats.size() > 1)
	{
		return availableFormats.at(1);
	}

	return availableFormats.at(0);
}

vk::PresentModeKHR VulkanSwapchain::SelectPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) const noexcept
{
	assert(!availableModes.empty());

	for (const auto& availableMode : availableModes)
	{
		bool niceMode = vk::PresentModeKHR::eImmediate == availableMode;

		if (niceMode)
		{
			return availableMode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanSwapchain::SelectSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept
{
	if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		vk::Extent2D actualExtent = { mWindowSize.x, mWindowSize.y };
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void VulkanSwapchain::CreateFramebuffers(VulkanRenderPass& renderPass, VulkanImage& depthImage, VulkanImage& resolveImage)
{
	mFramebuffers.reserve(mImages.size());

	for (const auto& image : mImages)
	{
		vk::ImageView attachments[] = { resolveImage.GetImageView(), depthImage.GetImageView(), image->GetImageView() };

		auto framebufferCreateInfo = vk::FramebufferCreateInfo()
			.setRenderPass(renderPass.Handle().get())
			.setAttachmentCount(static_cast<std::uint32_t>(std::size(attachments)))
			.setPAttachments(attachments)
			.setWidth(mExtent.width)
			.setHeight(mExtent.height)
			.setLayers(1);

		mFramebuffers.push_back(mDevice.Handle()->createFramebufferUnique(framebufferCreateInfo));
	}

	LoggerInfo << "Framebuffers created";
}

vk::Extent2D VulkanSwapchain::GetExtent() const noexcept 
{ 
	return mExtent; 
}

vk::Format VulkanSwapchain::GetImageFormat() const noexcept 
{
	return mFormat; 
}

const std::vector<std::unique_ptr<VulkanImage>>& VulkanSwapchain::GetImages() const noexcept
{ 
	return mImages; 
}

std::size_t VulkanSwapchain::GetImageCount() const noexcept 
{ 
	return mImages.size(); 
}

}
