#include "VulkanSwapchain.h"

#include <Utils/Logger.hpp>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanRenderPass.h>

namespace Lucid::Vulkan
{

VulkanSwapchain::VulkanSwapchain(
    VulkanDevice& device,
    const VulkanSurface& surface,
    const Core::Vector2d<std::uint32_t>& size,
    const std::string& name)
    : VulkanEntity(name, device.Handle())
    , mSurface(surface)
    , mWindowSize(size)
{
    VulkanDevice::SwapchainDetails details = device.GetSwapchainDetails(mSurface);

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
                                   .setSurface(mSurface.Handle())
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

    std::uint32_t queueFamilies[]
        = { device.FindGraphicsQueueFamily().value(), device.FindPresentQueueFamily(mSurface).value() };

    if (queueFamilies[0] != queueFamilies[1])
    {
        swapchainCreateInfo.setQueueFamilyIndexCount(2)
            .setPQueueFamilyIndices(queueFamilies)
            .setImageSharingMode(vk::SharingMode::eConcurrent);
    }
    else
    {
        swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
    }

    VulkanEntity::SetHandle(Device().createSwapchainKHRUnique(swapchainCreateInfo));
    LoggerInfo << "Swapchain created";

    auto swapchainImages = Device().getSwapchainImagesKHR(Handle());
    for (const auto& image : swapchainImages)
    {
        mImages.push_back(
            VulkanImage::FromSwapchain(device, image, mFormat, vk::ImageAspectFlagBits::eColor, "Swapchain image"));
    }
}

vk::ResultValue<std::uint32_t>
VulkanSwapchain::AcquireNextImage(const vk::UniqueSemaphore& semaphore)
{
    return Device().acquireNextImageKHR(Handle(), std::numeric_limits<std::uint64_t>::max(), semaphore.get(), {});
}

const std::vector<vk::UniqueFramebuffer>&
VulkanSwapchain::GetFramebuffers() const noexcept
{
    return mFramebuffers;
}

vk::SurfaceFormatKHR
VulkanSwapchain::SelectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const noexcept
{
    assert(!availableFormats.empty());

    for (const auto& availableFormat : availableFormats)
    {
        bool niceFormat = vk::Format::eR16G16B16A16Sfloat == availableFormat.format;
        bool niceColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear == availableFormat.colorSpace;

        if (niceFormat && niceColorSpace)
        {
            LoggerInfo << "Selected nice colorspace and format";
            return availableFormat;
        }
    }

    LoggerWarning << "Selected available colorspace and format";

    if (availableFormats.size() > 1)
    {
        return availableFormats.at(1);
    }

    return availableFormats.at(0);
}

vk::PresentModeKHR
VulkanSwapchain::SelectPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) const noexcept
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

vk::Extent2D
VulkanSwapchain::SelectSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept
{
    if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        vk::Extent2D actualExtent = { mWindowSize.x, mWindowSize.y };
        actualExtent.width
            = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height
            = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void
VulkanSwapchain::CreateFramebuffers(VulkanRenderPass& renderPass, VulkanImage& depthImage, VulkanImage& resolveImage)
{
    mFramebuffers.reserve(mImages.size());

    for (const auto& image : mImages)
    {
        vk::ImageView attachments[] = { resolveImage.GetImageView(), depthImage.GetImageView(), image->GetImageView() };

        auto framebufferCreateInfo = vk::FramebufferCreateInfo()
                                         .setRenderPass(renderPass.Handle())
                                         .setAttachmentCount(static_cast<std::uint32_t>(std::size(attachments)))
                                         .setPAttachments(attachments)
                                         .setWidth(mExtent.width)
                                         .setHeight(mExtent.height)
                                         .setLayers(1);

        mFramebuffers.push_back(Device().createFramebufferUnique(framebufferCreateInfo));
    }

    LoggerInfo << "Framebuffers created";
}

vk::Extent2D
VulkanSwapchain::GetExtent() const noexcept
{
    return mExtent;
}

vk::Format
VulkanSwapchain::GetImageFormat() const noexcept
{
    return mFormat;
}

const std::vector<std::unique_ptr<VulkanImage>>&
VulkanSwapchain::GetImages() const noexcept
{
    return mImages;
}

std::size_t
VulkanSwapchain::GetImageCount() const noexcept
{
    return mImages.size();
}

} // namespace Lucid::Vulkan
