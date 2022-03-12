#pragma once

#include <optional>
#include <set>
#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanEntity.h>

namespace Lucid::Vulkan
{

class VulkanSurface;

class VulkanDevice : public VulkanEntity<vk::UniqueDevice>
{
public:
    struct QueueFamilies
    {
        std::optional<std::uint32_t> graphics;
        std::optional<std::uint32_t> present;

        [[nodiscard]] bool IsComplete() const noexcept;
        [[nodiscard]] std::set<std::uint32_t> UniqueQueues() const noexcept;
    };

    struct SwapchainDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;

        [[nodiscard]] bool IsComplete() const noexcept;
    };

public:
    VulkanDevice(const vk::PhysicalDevice& device);
    void InitLogicalDeviceForSurface(const VulkanSurface& surface) noexcept;

    [[nodiscard]] bool IsSuitableForSurface(const VulkanSurface& surface) const noexcept;
    [[nodiscard]] SwapchainDetails GetSwapchainDetails(const VulkanSurface& surface) const noexcept;
    [[nodiscard]] std::optional<std::uint32_t> FindGraphicsQueueFamily() const noexcept;
    [[nodiscard]] std::optional<std::uint32_t> FindPresentQueueFamily(const VulkanSurface& surface) const noexcept;
    [[nodiscard]] vk::Queue& GetGraphicsQueue() noexcept;
    [[nodiscard]] vk::Queue& GetPresentQueue() noexcept;
    [[nodiscard]] vk::PhysicalDevice& GetPhysicalDevice() noexcept;
    [[nodiscard]] vk::Format FindSupportedDepthFormat();
    [[nodiscard]] bool DoesSupportBlitting(vk::Format format);
    [[nodiscard]] vk::SampleCountFlagBits GetMsaaSamples() const;

private:
    [[nodiscard]] std::vector<const char*> GetUnsupportedExtensions() const noexcept;

private:
    vk::PhysicalDevice mPhysicalDevice;
    vk::Queue mGraphicsQueue;
    vk::Queue mPresentQueue;
    vk::SampleCountFlagBits mMsaaSamples;

    const std::vector<const char*> mExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset" };
};

}