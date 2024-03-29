#pragma once

#include <filesystem>
#include <optional>

#include <Core/Interfaces.h>
#include <Vulkan/VulkanEntity.h>
#include <vulkan/vulkan.hpp>

namespace Lucid::Core
{
struct Texture;
}

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanCommandPool;
class VulkanBuffer;

/*
        Wrapper around Vulkan image. Could create image from swapchain or from disk.
        Holds vk::UniqueImage for user-created images, since swapchain images are none unique.
*/
class VulkanImage : public VulkanEntity<vk::Image>
{
public:
    static std::unique_ptr<VulkanImage> CreateDepthImage(
        VulkanDevice& device,
        const vk::Extent2D& swapchainExtent,
        vk::Format format,
        vk::ImageAspectFlags aspectFlags);

    static std::unique_ptr<VulkanImage>
    FromSwapchain(VulkanDevice& device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);

    static std::unique_ptr<VulkanImage> FromTexture(
        VulkanDevice& device,
        VulkanCommandPool& commandPool,
        const Core::TexturePtr& texture,
        vk::Format format,
        vk::ImageAspectFlags aspectFlags);

    static std::unique_ptr<VulkanImage> FromCubemap(
        VulkanDevice& device,
        VulkanCommandPool& commandPool,
        const std::array<Core::TexturePtr, 6>& textures,
        vk::Format format,
        vk::ImageAspectFlags aspectFlags);

    static std::unique_ptr<VulkanImage>
    CreateImage(VulkanDevice& device, vk::Format format, const vk::Extent2D& swapchainExtent);

    void Transition(
        VulkanCommandPool& commandPool,
        vk::Format format,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout,
        std::size_t layerCount = 1);

    void Write(
        VulkanCommandPool& commandPool,
        const VulkanBuffer& buffer,
        const Core::Vector2d<std::uint32_t>& size,
        std::size_t layerCount = 1);

    [[nodiscard]] const vk::ImageView& GetImageView() const;
    [[nodiscard]] bool HasStencil(vk::Format format) const;
    [[nodiscard]] std::uint32_t GetMipLevels() const;

private:
    VulkanImage(
        VulkanDevice& device,
        std::uint32_t width,
        std::uint32_t height,
        vk::Format format,
        vk::ImageTiling tiling,
        vk::ImageUsageFlags usage,
        vk::MemoryPropertyFlags memoryProperty);

    VulkanImage(VulkanDevice& device, vk::Image image);

    VulkanImage(VulkanDevice& device, VulkanCommandPool& commandPool, const Core::TexturePtr& texture);

    VulkanImage(VulkanDevice& device, VulkanCommandPool& commandPool, const std::array<Core::TexturePtr, 6>& textures);

    void GenerateImageView(
        vk::Format format,
        vk::ImageAspectFlags aspectFlags,
        vk::ImageViewType viewType,
        std::size_t layerCount = 1);

    void GenerateMipmaps(
        VulkanCommandPool& commandPool,
        const Core::Vector2d<std::uint32_t>& size,
        vk::Format format,
        std::size_t layerCount = 1);

    VulkanDevice& mDevice;
    vk::UniqueDeviceMemory mDeviceMemory;
    vk::UniqueImageView mImageView;
    std::optional<vk::UniqueImage> mUniqueImageHolder;
    std::uint32_t mMipLevels = 1;
};

} // namespace Lucid::Vulkan
