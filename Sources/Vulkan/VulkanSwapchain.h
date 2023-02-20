#pragma once

#include <Core/Interfaces.h>
#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanEntity.h>
#include <Vulkan/VulkanImage.h>
#include <Vulkan/VulkanSurface.h>
#include <vulkan/vulkan.hpp>

namespace Lucid::Vulkan
{

class VulkanRenderPass;
class VulkanUniformBuffer;

class VulkanSwapchain : public VulkanEntity<vk::UniqueSwapchainKHR>
{
public:
    VulkanSwapchain(VulkanDevice& device, const VulkanSurface& surface, const Core::Vector2d<std::uint32_t>& size);
    void CreateFramebuffers(VulkanRenderPass& renderPass, VulkanImage& depthImage, VulkanImage& resolveImage);

    [[nodiscard]] vk::Extent2D GetExtent() const noexcept;
    [[nodiscard]] vk::Format GetImageFormat() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VulkanImage>>& GetImages() const noexcept;
    [[nodiscard]] std::size_t GetImageCount() const noexcept;
    [[nodiscard]] vk::ResultValue<std::uint32_t> AcquireNextImage(const vk::UniqueSemaphore& semaphore);
    [[nodiscard]] const std::vector<vk::UniqueFramebuffer>& GetFramebuffers() const noexcept;

private:
    [[nodiscard]] vk::SurfaceFormatKHR
    SelectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const noexcept;
    [[nodiscard]] vk::PresentModeKHR
    SelectPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) const noexcept;
    [[nodiscard]] vk::Extent2D SelectSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept;

    const VulkanSurface& mSurface;
    VulkanDevice& mDevice;
    Core::Vector2d<std::uint32_t> mWindowSize;

    vk::Format mFormat;
    vk::Extent2D mExtent;

    std::vector<std::unique_ptr<VulkanImage>> mImages;
    std::vector<vk::UniqueFramebuffer> mFramebuffers;
};

} // namespace Lucid::Vulkan
