#include "VulkanImage.h"

#include <Core/Texture.h>
#include <Utils/Files.h>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanCommandPool.h>
#include <Vulkan/VulkanDevice.h>

namespace Lucid::Vulkan
{

VulkanImage::VulkanImage(VulkanDevice& device, VulkanCommandPool& commandPool, const Core::Texture& texture)
    : mDevice(device)
{
    VulkanBuffer stagingBuffer(
        device,
        texture.pixels.size(),
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    stagingBuffer.Write(const_cast<char*>(texture.pixels.data()));

    auto createInfo = vk::ImageCreateInfo()
                          .setImageType(vk::ImageType::e2D)
                          .setExtent(vk::Extent3D().setWidth(texture.size.x).setHeight(texture.size.y).setDepth(1))
                          .setMipLevels(1)
                          .setArrayLayers(1)
                          .setFormat(vk::Format::eR8G8B8A8Srgb)
                          .setTiling(vk::ImageTiling::eOptimal)
                          .setInitialLayout(vk::ImageLayout::eUndefined)
                          .setUsage(
                              vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
                              | vk::ImageUsageFlagBits::eTransferSrc)
                          .setSharingMode(vk::SharingMode::eExclusive)
                          .setSamples(vk::SampleCountFlagBits::e1)
                          .setMipLevels(texture.mipLevels);

    mUniqueImageHolder = device.Handle()->createImageUnique(createInfo);
    mHandle = mUniqueImageHolder.value().get();
    mMipLevels = texture.mipLevels;

    vk::MemoryRequirements requirements = device.Handle()->getImageMemoryRequirements(Handle());
    std::uint32_t memoryType
        = VulkanBuffer::FindMemoryType(device, requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

    auto allocateInfo = vk::MemoryAllocateInfo().setAllocationSize(requirements.size).setMemoryTypeIndex(memoryType);

    mDeviceMemory = device.Handle()->allocateMemoryUnique(allocateInfo);
    device.Handle()->bindImageMemory(Handle(), mDeviceMemory.get(), 0);

    Transition(
        commandPool, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    Write(commandPool, stagingBuffer, texture.size);
    GenerateMipmaps(commandPool, texture.size, vk::Format::eR8G8B8A8Srgb);
}

std::unique_ptr<VulkanImage>
VulkanImage::CreateDepthImage(
    VulkanDevice& device,
    const vk::Extent2D& swapchainExtent,
    vk::Format format,
    vk::ImageAspectFlags aspectFlags)
{
    VulkanImage result(
        device,
        swapchainExtent.width,
        swapchainExtent.height,
        device.FindSupportedDepthFormat(),
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal);

    result.GenerateImageView(format, aspectFlags);
    return std::make_unique<VulkanImage>(std::move(result));
}

std::unique_ptr<VulkanImage>
VulkanImage::FromSwapchain(VulkanDevice& device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags)
{
    VulkanImage result(device, image);
    result.GenerateImageView(format, aspectFlags);
    return std::make_unique<VulkanImage>(std::move(result));
}

std::unique_ptr<VulkanImage>
VulkanImage::FromTexture(
    VulkanDevice& device,
    VulkanCommandPool& commandPool,
    const Core::Texture& texture,
    vk::Format format,
    vk::ImageAspectFlags aspectFlags)
{
    VulkanImage result(device, commandPool, texture);
    result.GenerateImageView(format, aspectFlags);
    return std::make_unique<VulkanImage>(std::move(result));
}

std::unique_ptr<VulkanImage>
VulkanImage::CreateImage(VulkanDevice& device, vk::Format format, const vk::Extent2D& swapchainExtent)
{
    VulkanImage result(
        device,
        swapchainExtent.width,
        swapchainExtent.height,
        format,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal);
    result.GenerateImageView(format, vk::ImageAspectFlagBits::eColor);
    return std::make_unique<VulkanImage>(std::move(result));
}

VulkanImage::VulkanImage(
    VulkanDevice& device,
    std::uint32_t width,
    std::uint32_t height,
    vk::Format format,
    vk::ImageTiling tiling,
    vk::ImageUsageFlags usage,
    vk::MemoryPropertyFlags memoryProperty)
    : mDevice(device)
{
    auto createInfo = vk::ImageCreateInfo()
                          .setImageType(vk::ImageType::e2D)
                          .setExtent(vk::Extent3D().setWidth(width).setHeight(height).setDepth(1))
                          .setMipLevels(1)
                          .setArrayLayers(1)
                          .setFormat(format)
                          .setTiling(tiling)
                          .setInitialLayout(vk::ImageLayout::eUndefined)
                          .setUsage(usage)
                          .setSharingMode(vk::SharingMode::eExclusive)
                          .setSamples(device.GetMsaaSamples());

    mUniqueImageHolder = device.Handle()->createImageUnique(createInfo);
    mHandle = mUniqueImageHolder.value().get();

    vk::MemoryRequirements requirements = device.Handle()->getImageMemoryRequirements(Handle());
    std::uint32_t memoryType = VulkanBuffer::FindMemoryType(device, requirements.memoryTypeBits, memoryProperty);

    auto allocateInfo = vk::MemoryAllocateInfo().setAllocationSize(requirements.size).setMemoryTypeIndex(memoryType);

    mDeviceMemory = device.Handle()->allocateMemoryUnique(allocateInfo);
    device.Handle()->bindImageMemory(Handle(), mDeviceMemory.get(), 0);
}

VulkanImage::VulkanImage(VulkanDevice& device, vk::Image image)
    : mDevice(device)
{
    mHandle = image;
}

void
VulkanImage::Transition(
    VulkanCommandPool& commandPool,
    vk::Format format,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout)
{
    commandPool.ExecuteSingleCommand(
        [&, this](vk::CommandBuffer& commandBuffer)
        {
            auto barrier = vk::ImageMemoryBarrier()
                               .setOldLayout(oldLayout)
                               .setNewLayout(newLayout)
                               .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                               .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                               .setImage(Handle())
                               .setSubresourceRange(vk::ImageSubresourceRange()
                                                        .setBaseMipLevel(0)
                                                        .setLayerCount(1)
                                                        .setBaseArrayLayer(0)
                                                        .setLevelCount(mMipLevels));

            vk::PipelineStageFlags sourceStage;
            vk::PipelineStageFlags destinationStage;

            if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
            {
                barrier.setSrcAccessMask({}).setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

                sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
                destinationStage = vk::PipelineStageFlagBits::eTransfer;
            }
            else if (
                oldLayout == vk::ImageLayout::eTransferDstOptimal
                && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
            {
                barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

                sourceStage = vk::PipelineStageFlagBits::eTransfer;
                destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
            }
            else
            {
                throw std::runtime_error("Unsupported transition");
            }

            // Handle depth and color aspect mask
            if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
            {
                barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

                if (HasStencil(format))
                {
                    barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
                }
            }
            else
            {
                barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            }

            commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);
        });
}

void
VulkanImage::Write(
    VulkanCommandPool& commandPool,
    const VulkanBuffer& buffer,
    const Core::Vector2d<std::uint32_t>& size)
{
    commandPool.ExecuteSingleCommand(
        [&, this](vk::CommandBuffer& commandBuffer)
        {
            auto region = vk::BufferImageCopy()
                              .setBufferOffset(0)
                              .setBufferRowLength(0)
                              .setBufferImageHeight(0)
                              .setImageSubresource(vk::ImageSubresourceLayers()
                                                       .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                                       .setMipLevel(0)
                                                       .setBaseArrayLayer(0)
                                                       .setLayerCount(1))
                              .setImageOffset({ 0, 0, 0 })
                              .setImageExtent({ size.x, size.y, 1 });

            commandBuffer.copyBufferToImage(
                buffer.Handle().get(), Handle(), vk::ImageLayout::eTransferDstOptimal, region);
        });
}

const vk::ImageView&
VulkanImage::GetImageView() const
{
    return mImageView.get();
}

void
VulkanImage::GenerateImageView(vk::Format format, vk::ImageAspectFlags aspectFlags)
{
    auto imageViewCreateInfo = vk::ImageViewCreateInfo()
                                   .setImage(Handle())
                                   .setViewType(vk::ImageViewType::e2D)
                                   .setFormat(format)
                                   .setComponents(vk::ComponentMapping {}) // Identity by default
                                   .setSubresourceRange(vk::ImageSubresourceRange {}
                                                            .setAspectMask(aspectFlags)
                                                            .setBaseArrayLayer(0)
                                                            .setBaseMipLevel(0)
                                                            .setLayerCount(1)
                                                            .setLevelCount(mMipLevels));

    mImageView = mDevice.Handle()->createImageViewUnique(imageViewCreateInfo);
}

void
VulkanImage::GenerateMipmaps(
    VulkanCommandPool& commandPool,
    const Core::Vector2d<std::uint32_t>& size,
    vk::Format format)
{
    if (!mDevice.DoesSupportBlitting(format))
    {
        throw std::runtime_error("Device doesn't support blitting");
    }

    commandPool.ExecuteSingleCommand(
        [&, this](vk::CommandBuffer& commandBuffer)
        {
            auto barrier = vk::ImageMemoryBarrier()
                               .setImage(Handle())
                               .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                               .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                               .setSubresourceRange(vk::ImageSubresourceRange()
                                                        .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                                        .setLayerCount(1)
                                                        .setLevelCount(1)
                                                        .setBaseArrayLayer(0));

            std::uint32_t mipWidth = size.x;
            std::uint32_t mipHeight = size.y;

            for (std::uint32_t i = 1; i < mMipLevels; i++)
            {
                barrier.subresourceRange.setBaseMipLevel(i - 1);
                barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                    .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
                    .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eTransferRead);

                commandBuffer.pipelineBarrier(
                    vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, barrier);

                auto srcOffsets = std::array<vk::Offset3D, 2> {
                    vk::Offset3D(0, 0, 0),
                    vk::Offset3D(static_cast<std::int32_t>(mipWidth), static_cast<std::int32_t>(mipHeight), 1)
                };
                auto dstOffsets = std::array<vk::Offset3D, 2> {
                    vk::Offset3D(0, 0, 0),
                    vk::Offset3D(mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1)
                };

                auto blit = vk::ImageBlit()
                                .setSrcOffsets(srcOffsets)
                                .setSrcSubresource(vk::ImageSubresourceLayers()
                                                       .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                                       .setMipLevel(i - 1)
                                                       .setBaseArrayLayer(0)
                                                       .setLayerCount(1))
                                .setDstOffsets(dstOffsets)
                                .setDstSubresource(vk::ImageSubresourceLayers()
                                                       .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                                       .setMipLevel(i)
                                                       .setBaseArrayLayer(0)
                                                       .setLayerCount(1));

                commandBuffer.blitImage(
                    Handle(),
                    vk::ImageLayout::eTransferSrcOptimal,
                    Handle(),
                    vk::ImageLayout::eTransferDstOptimal,
                    blit,
                    vk::Filter::eLinear);

                barrier.setOldLayout(vk::ImageLayout::eTransferSrcOptimal)
                    .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

                commandBuffer.pipelineBarrier(
                    vk::PipelineStageFlagBits::eTransfer,
                    vk::PipelineStageFlagBits::eFragmentShader,
                    {},
                    {},
                    {},
                    barrier);

                if (mipWidth > 1)
                {
                    mipWidth /= 2;
                }

                if (mipHeight > 1)
                {
                    mipHeight /= 2;
                }
            }

            barrier.subresourceRange.setBaseMipLevel(mMipLevels - 1);
            barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrier);
        });
}

bool
VulkanImage::HasStencil(vk::Format format) const
{
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

std::uint32_t
VulkanImage::GetMipLevels() const
{
    return mMipLevels;
}

} // namespace Lucid::Vulkan
