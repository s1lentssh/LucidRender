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
    [[nodiscard]] bool IsSuitableForSurface(const VulkanSurface& surface) const noexcept;
    void InitLogicalDeviceForSurface(const VulkanSurface& surface) noexcept;
    [[nodiscard]] SwapchainDetails GetSwapchainDetails(const VulkanSurface& surface) const noexcept;
    [[nodiscard]] std::optional<std::uint32_t> FindGraphicsQueueFamily() const noexcept;
    [[nodiscard]] std::optional<std::uint32_t> FindPresentQueueFamily(const VulkanSurface& surface) const noexcept;
    [[nodiscard]] vk::Queue& GetGraphicsQueue() noexcept;
    [[nodiscard]] vk::Queue& GetPresentQueue() noexcept;
    [[nodiscard]] vk::PhysicalDevice& GetPhysicalDevice() noexcept;

private:
    [[nodiscard]] std::vector<const char*> GetUnsupportedExtensions() const noexcept;

private:
    vk::PhysicalDevice mPhysicalDevice;
    vk::Queue mGraphicsQueue;
    vk::Queue mPresentQueue;

    const std::vector<const char*> mExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};

}