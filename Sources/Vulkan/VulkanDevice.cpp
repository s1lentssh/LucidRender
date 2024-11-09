#include "VulkanDevice.h"

#include <iostream>

#include <Utils/Logger.hpp>
#include <Vulkan/VulkanSurface.h>

namespace Lucid::Vulkan
{

VulkanDevice::VulkanDevice(const vk::PhysicalDevice& device)
    : mPhysicalDevice(device)
{
    auto properties = device.getProperties();
    vk::SampleCountFlags flags
        = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;

    if (flags & vk::SampleCountFlagBits::e64)
    {
        mMsaaSamples = vk::SampleCountFlagBits::e64;
    }
    else if (flags & vk::SampleCountFlagBits::e32)
    {
        mMsaaSamples = vk::SampleCountFlagBits::e32;
    }
    else if (flags & vk::SampleCountFlagBits::e16)
    {
        mMsaaSamples = vk::SampleCountFlagBits::e16;
    }
    else if (flags & vk::SampleCountFlagBits::e8)
    {
        mMsaaSamples = vk::SampleCountFlagBits::e8;
    }
    else if (flags & vk::SampleCountFlagBits::e4)
    {
        mMsaaSamples = vk::SampleCountFlagBits::e4;
    }
    else if (flags & vk::SampleCountFlagBits::e2)
    {
        mMsaaSamples = vk::SampleCountFlagBits::e2;
    }
    else
    {
        mMsaaSamples = vk::SampleCountFlagBits::e1;
    }
}

bool
VulkanDevice::IsSuitableForSurface(const VulkanSurface& surface) const noexcept
{
    vk::PhysicalDeviceProperties properties = mPhysicalDevice.getProperties();
    vk::PhysicalDeviceFeatures features = mPhysicalDevice.getFeatures();

    bool isSuitableDeviceType = properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu
        || properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu;
    bool supportsGeometry = features.geometryShader || true;
    bool hasGraphicsQueueFamily = FindGraphicsQueueFamily().has_value();
    bool hasPresentQueueFamily = FindPresentQueueFamily(surface).has_value();
    bool allExtensionsSupported = GetUnsupportedExtensions().empty();
    bool hasSwapchainSupport = GetSwapchainDetails(surface).IsComplete();
    bool hasSamplerAnisotropy = features.samplerAnisotropy;

    return isSuitableDeviceType && supportsGeometry && hasGraphicsQueueFamily && hasPresentQueueFamily
        && allExtensionsSupported && hasSwapchainSupport && hasSamplerAnisotropy;
}

void
VulkanDevice::InitLogicalDeviceForSurface(const VulkanSurface& surface) noexcept
{
    QueueFamilies queueFamilies = { FindGraphicsQueueFamily(), FindPresentQueueFamily(surface) };

    auto deviceFeatures
        = vk::PhysicalDeviceFeatures().setFillModeNonSolid(true).setSamplerAnisotropy(true).setSampleRateShading(true);

    const float queuePriority = 1.0f;

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    for (const std::uint32_t queueFamily : queueFamilies.UniqueQueues())
    {
        auto queueCreateInfo = vk::DeviceQueueCreateInfo()
                                   .setQueueFamilyIndex(queueFamily)
                                   .setQueueCount(1)
                                   .setPQueuePriorities(&queuePriority);

        queueCreateInfos.push_back(queueCreateInfo);
    }

    auto deviceCreateInfo = vk::DeviceCreateInfo()
                                .setPQueueCreateInfos(queueCreateInfos.data())
                                .setQueueCreateInfoCount(static_cast<std::uint32_t>(queueCreateInfos.size()))
                                .setPEnabledFeatures(&deviceFeatures)
                                .setEnabledExtensionCount(static_cast<std::uint32_t>(mExtensions.size()))
                                .setPpEnabledExtensionNames(mExtensions.data());

    mHandle = mPhysicalDevice.createDeviceUnique(deviceCreateInfo);
    LoggerInfo << "Logical device created";

    mGraphicsQueue = Handle()->getQueue(queueFamilies.graphics.value(), 0);
    mPresentQueue = Handle()->getQueue(queueFamilies.present.value(), 0);
}

std::optional<std::uint32_t>
VulkanDevice::FindGraphicsQueueFamily() const noexcept
{
    std::vector<vk::QueueFamilyProperties> queueFamiliesProperties = mPhysicalDevice.getQueueFamilyProperties();
    for (std::uint32_t i = 0; i < queueFamiliesProperties.size(); ++i)
    {
        const vk::QueueFamilyProperties& queueFamily = queueFamiliesProperties.at(i);
        const bool supportGraphics = static_cast<bool>(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics);

        if (supportGraphics)
        {
            return i;
        }
    }

    return {};
}

std::optional<std::uint32_t>
VulkanDevice::FindPresentQueueFamily(const VulkanSurface& surface) const noexcept
{
    std::vector<vk::QueueFamilyProperties> queueFamiliesProperties = mPhysicalDevice.getQueueFamilyProperties();
    for (std::uint32_t i = 0; i < queueFamiliesProperties.size(); ++i)
    {
        vk::Bool32 supportsSurface = mPhysicalDevice.getSurfaceSupportKHR(i, surface.Handle().get());

        if (supportsSurface)
        {
            return i;
        }
    }

    return false;
}

vk::Queue&
VulkanDevice::GetGraphicsQueue() noexcept
{
    return mGraphicsQueue;
}

vk::Queue&
VulkanDevice::GetPresentQueue() noexcept
{
    return mPresentQueue;
}

vk::PhysicalDevice&
VulkanDevice::GetPhysicalDevice() noexcept
{
    return mPhysicalDevice;
}

VulkanDevice::SwapchainDetails
VulkanDevice::GetSwapchainDetails(const VulkanSurface& surface) const noexcept
{
    SwapchainDetails details;

    details.formats = mPhysicalDevice.getSurfaceFormatsKHR(surface.Handle().get());
    details.capabilities = mPhysicalDevice.getSurfaceCapabilitiesKHR(surface.Handle().get());
    details.presentModes = mPhysicalDevice.getSurfacePresentModesKHR(surface.Handle().get());

    return details;
}

bool
VulkanDevice::QueueFamilies::IsComplete() const noexcept
{
    return graphics.has_value() && present.has_value();
}

std::set<std::uint32_t>
VulkanDevice::QueueFamilies::UniqueQueues() const noexcept
{
    return { graphics.value(), present.value() };
}

std::vector<const char*>
VulkanDevice::GetUnsupportedExtensions() const noexcept
{
    std::vector<vk::ExtensionProperties> availableExtensions = mPhysicalDevice.enumerateDeviceExtensionProperties();
    std::vector<const char*> requiredExtensions = mExtensions;

    // Remove required extension from list if it's available
    for (const auto& availableExtension : availableExtensions)
    {
        requiredExtensions.erase(
            std::remove_if(
                requiredExtensions.begin(),
                requiredExtensions.end(),
                [&availableExtension](const char* extension)
                { return std::string(availableExtension.extensionName.data()) == std::string(extension); }),
            requiredExtensions.end());
    }

    return requiredExtensions;
}

bool
VulkanDevice::SwapchainDetails::IsComplete() const noexcept
{
    return !formats.empty() && !presentModes.empty();
}

vk::Format
VulkanDevice::FindSupportedDepthFormat()
{
    const std::vector<vk::Format>& candidates
        = { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint };

    const vk::ImageTiling selectedTiling = vk::ImageTiling::eOptimal;
    const vk::FormatFeatureFlags features = vk::FormatFeatureFlagBits::eDepthStencilAttachment;

    for (const auto& format : candidates)
    {
        vk::FormatProperties properties = GetPhysicalDevice().getFormatProperties(format);
        switch (selectedTiling)
        {
        case vk::ImageTiling::eOptimal:
            if ((properties.optimalTilingFeatures & features) == features)
            {
                return format;
            }
            break;
        case vk::ImageTiling::eLinear:
            if ((properties.linearTilingFeatures & features) == features)
            {
                return format;
            }
            break;
        default:
            break;
        }
    }

    throw std::runtime_error("Failed to find supported depth format");
}

bool
VulkanDevice::DoesSupportBlitting(vk::Format format)
{
    auto properties = GetPhysicalDevice().getFormatProperties(format);
    return static_cast<bool>(properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear);
}

vk::SampleCountFlagBits
VulkanDevice::GetMsaaSamples() const
{
    return mMsaaSamples;
}

} // namespace Lucid::Vulkan
