#pragma once

#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanDevice.h>
#include <Utils/Interfaces.hpp>

namespace lucid {

class VulkanSwapchain
{
public:
    VulkanSwapchain(VulkanDevice& device, const VulkanSurface& surface, const IWindow& window);
    [[nodiscard]] vk::Extent2D GetExtent() const noexcept { return mExtent; }
    [[nodiscard]] vk::Format GetImageFormat() const noexcept { return mFormat; }
    [[nodiscard]] const std::vector<vk::UniqueImageView>& GetImageViews() const noexcept { return mImageViews; }

private:
    [[nodiscard]] vk::SurfaceFormatKHR SelectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const noexcept;
    [[nodiscard]] vk::PresentModeKHR SelectPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) const noexcept;
    [[nodiscard]] vk::Extent2D SelectSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept;

    void Init();
    void CreateImageViews();

    const IWindow& mWindow;
    VulkanDevice& mDevice;
    const VulkanSurface& mSurface;

    vk::UniqueSwapchainKHR mSwapchain;
    std::vector<vk::Image> mImages;
    std::vector<vk::UniqueImageView> mImageViews;
    vk::Format mFormat;
    vk::Extent2D mExtent;
};

}