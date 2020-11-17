#pragma once

#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanSurface.h>
#include <Vulkan/VulkanDevice.h>

namespace Lucid {

class IWindow;
class VulkanRenderPass;

class VulkanSwapchain
{
public:
    VulkanSwapchain(VulkanDevice& device, const VulkanSurface& surface, const IWindow& window);
    void CreateFramebuffers(VulkanRenderPass& renderPass);

    [[nodiscard]] vk::Extent2D GetExtent() const noexcept { return mExtent; }
    [[nodiscard]] vk::Format GetImageFormat() const noexcept { return mFormat; }
    [[nodiscard]] const std::vector<vk::UniqueImageView>& GetImageViews() const noexcept { return mImageViews; }
    [[nodiscard]] const std::size_t GetImageCount() const noexcept { return mImageViews.size(); }
    [[nodiscard]] std::uint32_t AcquireNextImage(const vk::UniqueSemaphore& semaphore);
    [[nodiscard]] vk::UniqueSwapchainKHR& Handle() { return mSwapchain; }
    [[nodiscard]] std::vector<vk::UniqueFramebuffer>& GetFramebuffers() { return mFramebuffers; }

private:
    [[nodiscard]] vk::SurfaceFormatKHR SelectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const noexcept;
    [[nodiscard]] vk::PresentModeKHR SelectPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) const noexcept;
    [[nodiscard]] vk::Extent2D SelectSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept;

    void Init();
    void CreateImageViews();

    const IWindow& mWindow;
    const VulkanSurface& mSurface;
    VulkanDevice& mDevice;

    vk::UniqueSwapchainKHR mSwapchain;
    vk::Format mFormat;
    vk::Extent2D mExtent;

    std::vector<vk::Image> mImages;
    std::vector<vk::UniqueImageView> mImageViews;
    std::vector<vk::UniqueFramebuffer> mFramebuffers;
};

}