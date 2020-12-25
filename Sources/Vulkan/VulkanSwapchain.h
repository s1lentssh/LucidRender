#pragma once

#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanSurface.h>
#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanEntity.h>
#include <Utils/Interfaces.hpp>

namespace Lucid::Vulkan
{

class VulkanRenderPass;
class VulkanUniformBuffer;

class VulkanSwapchain : public VulkanEntity<vk::UniqueSwapchainKHR>
{
public:
    VulkanSwapchain(VulkanDevice& device, const VulkanSurface& surface, const Vector2d<std::uint32_t>& size);
    void CreateFramebuffers(VulkanRenderPass& renderPass);

    [[nodiscard]] vk::Extent2D GetExtent() const noexcept;
    [[nodiscard]] vk::Format GetImageFormat() const noexcept;
    [[nodiscard]] const std::vector<vk::UniqueImageView>& GetImageViews() const noexcept;
    [[nodiscard]] const std::size_t GetImageCount() const noexcept;
    [[nodiscard]] vk::ResultValue<std::uint32_t> AcquireNextImage(const vk::UniqueSemaphore& semaphore);
    [[nodiscard]] const std::vector<vk::UniqueFramebuffer>& GetFramebuffers() const noexcept;

private:
    [[nodiscard]] vk::SurfaceFormatKHR SelectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const noexcept;
    [[nodiscard]] vk::PresentModeKHR SelectPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) const noexcept;
    [[nodiscard]] vk::Extent2D SelectSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept;

    void Init();
    void CreateImageViews();

    const VulkanSurface& mSurface;
    VulkanDevice& mDevice;
    Vector2d<std::uint32_t> mWindowSize;

    vk::Format mFormat;
    vk::Extent2D mExtent;

    std::vector<vk::Image> mImages;
    std::vector<vk::UniqueImageView> mImageViews;
    std::vector<vk::UniqueFramebuffer> mFramebuffers;
};

}