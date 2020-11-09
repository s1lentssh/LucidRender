#include "VulkanSwapchain.h"
#include <Utils/Logger.hpp>
#include <Vulkan/VulkanSurface.h>

namespace lucid
{

VulkanSwapchain::VulkanSwapchain(VulkanDevice& device, const VulkanSurface& surface, const IWindow& window)
	: mWindow(window)
	, mDevice(device)
	, mSurface(surface)
{
	Init();
	CreateImageViews();
}

vk::SurfaceFormatKHR VulkanSwapchain::SelectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const noexcept
{
	assert(!availableFormats.empty());

	for (const auto& availableFormat : availableFormats)
	{
		bool niceFormat = vk::Format::eB8G8R8A8Srgb == availableFormat.format;
		bool niceColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear == availableFormat.colorSpace;

		if (niceFormat && niceColorSpace)
		{
			return availableFormat;
		}
	}

	return availableFormats.at(0);
}

vk::PresentModeKHR VulkanSwapchain::SelectPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) const noexcept
{
	assert(!availableModes.empty());

	for (const auto& availableMode : availableModes)
	{
		bool niceMode = vk::PresentModeKHR::eMailbox == availableMode;

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
		Vector2d windowSize = mWindow.GetSize();

		vk::Extent2D actualExtent = { windowSize.x, windowSize.y };
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void VulkanSwapchain::Init()
{
	VulkanDevice::SwapchainDetails details = mDevice.GetSwapchainDetails(mSurface);

	vk::SurfaceFormatKHR surfaceFormat = SelectSurfaceFormat(details.formats);
	vk::PresentModeKHR presentMode = SelectPresentMode(details.presentModes);
	vk::Extent2D extent = SelectSwapExtent(details.capabilities);

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

	mSwapchain = mDevice.Handle()->createSwapchainKHRUnique(swapchainCreateInfo);
	Logger::Info("Swapchain created");

	mImages = mDevice.Handle()->getSwapchainImagesKHR(mSwapchain.get());
	mFormat = surfaceFormat.format;
	mExtent = extent;
}

void VulkanSwapchain::CreateImageViews()
{
	mImageViews.reserve(mImages.size());

	for (const auto& image : mImages)
	{
		auto imageViewCreateInfo = vk::ImageViewCreateInfo()
			.setImage(image)
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(mFormat)
			.setComponents(vk::ComponentMapping{}) // Identity by default
			.setSubresourceRange(vk::ImageSubresourceRange{}
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseArrayLayer(0)
				.setBaseMipLevel(0)
				.setLayerCount(1)
				.setLevelCount(1));

		mImageViews.push_back(mDevice.Handle()->createImageViewUnique(imageViewCreateInfo));
	}
}

}
