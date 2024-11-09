#include "VulkanImage.h"

#include <numeric>

#include <Utils/Files.h>
#include <Utils/Logger.hpp>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanCommandPool.h>
#include <Vulkan/VulkanDevice.h>
#include <glm/gtc/type_ptr.hpp>

namespace Lucid::Vulkan
{

VulkanImage::VulkanImage(
    VulkanDevice& device,
    VulkanCommandPool& commandPool,
    const Core::TexturePtr& texture,
    const std::string& name)
    : VulkanEntity(name, device.Handle())
{
    VulkanBuffer stagingBuffer(
        device,
        texture->pixels.size(),
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        "Staging buffer for " + name);

    stagingBuffer.Write(texture->pixels.data());

    auto createInfo = vk::ImageCreateInfo()
                          .setImageType(vk::ImageType::e2D)
                          .setExtent(vk::Extent3D().setWidth(texture->size.x).setHeight(texture->size.y).setDepth(1))
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
                          .setMipLevels(texture->mipLevels);

    mUniqueImageHolder = Device().createImageUnique(createInfo);
    VulkanEntity::SetHandle(std::move(mUniqueImageHolder.value().get()));
    mMipLevels = texture->mipLevels;

    vk::MemoryRequirements requirements = Device().getImageMemoryRequirements(Handle());
    std::uint32_t memoryType = VulkanBuffer::FindMemoryType(
        device.GetPhysicalDevice(), requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

    auto allocateInfo = vk::MemoryAllocateInfo().setAllocationSize(requirements.size).setMemoryTypeIndex(memoryType);

    mDeviceMemory = Device().allocateMemoryUnique(allocateInfo);
    Device().bindImageMemory(Handle(), mDeviceMemory.get(), 0);

    Transition(
        commandPool, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    Write(commandPool, stagingBuffer, texture->size);
    GenerateMipmaps(commandPool, texture->size, vk::Format::eR8G8B8A8Srgb);
}

VulkanImage::VulkanImage(
    VulkanDevice& device,
    VulkanCommandPool& commandPool,
    const std::array<Core::TexturePtr, 6>& textures,
    const std::string& name)
    : VulkanEntity(name, device.Handle())
{
    (void)commandPool;

    std::size_t stagingSize = std::accumulate(
        textures.begin(),
        textures.end(),
        static_cast<std::size_t>(0),
        [](std::size_t accumulator, const Core::TexturePtr& value) { return accumulator + value->pixels.size(); });

    VulkanBuffer stagingBuffer(
        device,
        stagingSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        "Staging buffer for " + name);

    std::size_t offset { 0 };
    for (const auto& texture : textures)
    {
        stagingBuffer.Write(texture->pixels.data(), texture->pixels.size(), offset);
        offset += texture->pixels.size();
    }

    auto& firstTexture = textures.at(0);

    auto createInfo
        = vk::ImageCreateInfo()
              .setImageType(vk::ImageType::e2D)
              .setExtent(vk::Extent3D().setWidth(firstTexture->size.x).setHeight(firstTexture->size.y).setDepth(1))
              .setMipLevels(1)
              .setArrayLayers(static_cast<std::uint32_t>(textures.size()))
              .setFormat(vk::Format::eR8G8B8A8Srgb)
              .setTiling(vk::ImageTiling::eOptimal)
              .setInitialLayout(vk::ImageLayout::eUndefined)
              .setUsage(
                  vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
                  | vk::ImageUsageFlagBits::eTransferSrc)
              .setSharingMode(vk::SharingMode::eExclusive)
              .setSamples(vk::SampleCountFlagBits::e1)
              .setMipLevels(firstTexture->mipLevels)
              .setFlags(vk::ImageCreateFlagBits::eCubeCompatible);

    mUniqueImageHolder = Device().createImageUnique(createInfo);
    VulkanEntity::SetHandle(std::move(mUniqueImageHolder.value().get()));

    vk::MemoryRequirements requirements = Device().getImageMemoryRequirements(Handle());
    std::uint32_t memoryType = VulkanBuffer::FindMemoryType(
        device.GetPhysicalDevice(), requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

    auto allocateInfo = vk::MemoryAllocateInfo().setAllocationSize(requirements.size).setMemoryTypeIndex(memoryType);

    mDeviceMemory = Device().allocateMemoryUnique(allocateInfo);
    Device().bindImageMemory(Handle(), mDeviceMemory.get(), 0);

    Transition(
        commandPool,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        textures.size());

    Write(commandPool, stagingBuffer, firstTexture->size, textures.size());

    Transition(
        commandPool,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        textures.size());
}

VulkanImage::VulkanImage(
    VulkanDevice& device,
    VulkanCommandPool& commandPool,
    const glm::vec4& color,
    const std::string& name)
    : VulkanEntity(name, device.Handle())
{
    VulkanBuffer stagingBuffer(
        device,
        sizeof(float) * 4,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        "Staging buffer for " + name);

    std::vector<float> data(4);
    for (std::size_t i = 0; i < color.length(); i++)
    {
        data.at(i) = color[static_cast<std::int32_t>(i)];
    }
    stagingBuffer.Write(data.data());

    auto createInfo = vk::ImageCreateInfo()
                          .setImageType(vk::ImageType::e2D)
                          .setExtent(vk::Extent3D().setWidth(1).setHeight(1).setDepth(1))
                          .setMipLevels(1)
                          .setArrayLayers(1)
                          .setFormat(vk::Format::eR32G32B32A32Sfloat)
                          .setTiling(vk::ImageTiling::eOptimal)
                          .setInitialLayout(vk::ImageLayout::eUndefined)
                          .setUsage(
                              vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
                              | vk::ImageUsageFlagBits::eTransferSrc)
                          .setSharingMode(vk::SharingMode::eExclusive)
                          .setSamples(vk::SampleCountFlagBits::e1);

    mUniqueImageHolder = Device().createImageUnique(createInfo);
    VulkanEntity::SetHandle(std::move(mUniqueImageHolder.value().get()));
    mMipLevels = 1;

    vk::MemoryRequirements requirements = Device().getImageMemoryRequirements(Handle());
    std::uint32_t memoryType = VulkanBuffer::FindMemoryType(
        device.GetPhysicalDevice(), requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

    auto allocateInfo = vk::MemoryAllocateInfo().setAllocationSize(requirements.size).setMemoryTypeIndex(memoryType);

    mDeviceMemory = Device().allocateMemoryUnique(allocateInfo);
    Device().bindImageMemory(Handle(), mDeviceMemory.get(), 0);

    Transition(
        commandPool,
        vk::Format::eR32G32B32A32Sfloat,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal);
    Write(commandPool, stagingBuffer, { 1, 1 });
    GenerateMipmaps(commandPool, { 1, 1 }, vk::Format::eR32G32B32A32Sfloat);
}

std::unique_ptr<VulkanImage>
VulkanImage::CreateDepthImage(
    VulkanDevice& device,
    const vk::Extent2D& swapchainExtent,
    vk::Format format,
    vk::ImageAspectFlags aspectFlags,
    const std::string& name)
{
    VulkanImage result(
        device,
        swapchainExtent.width,
        swapchainExtent.height,
        device.FindSupportedDepthFormat(),
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        name);

    result.GenerateImageView(format, aspectFlags, vk::ImageViewType::e2D);
    return std::make_unique<VulkanImage>(std::move(result));
}

std::unique_ptr<VulkanImage>
VulkanImage::FromSwapchain(
    VulkanDevice& device,
    vk::Image image,
    vk::Format format,
    vk::ImageAspectFlags aspectFlags,
    const std::string& name)
{
    VulkanImage result(device, image, name);
    result.GenerateImageView(format, aspectFlags, vk::ImageViewType::e2D);
    return std::make_unique<VulkanImage>(std::move(result));
}

std::unique_ptr<VulkanImage>
VulkanImage::FromTexture(
    VulkanDevice& device,
    VulkanCommandPool& commandPool,
    const Core::TexturePtr& texture,
    vk::Format format,
    vk::ImageAspectFlags aspectFlags,
    const std::string& name)
{
    VulkanImage result(device, commandPool, texture, name);
    result.GenerateImageView(format, aspectFlags, vk::ImageViewType::e2D);
    return std::make_unique<VulkanImage>(std::move(result));
}

std::unique_ptr<VulkanImage>
VulkanImage::FromColor(
    VulkanDevice& device,
    VulkanCommandPool& commandPool,
    const glm::vec4& color,
    vk::Format format,
    vk::ImageAspectFlags aspectFlags,
    const std::string& name)
{
    VulkanImage result(device, commandPool, color, name);
    result.GenerateImageView(format, aspectFlags, vk::ImageViewType::e2D);
    return std::make_unique<VulkanImage>(std::move(result));
}

std::unique_ptr<VulkanImage>
VulkanImage::FromCubemap(
    VulkanDevice& device,
    VulkanCommandPool& commandPool,
    const std::array<Core::TexturePtr, 6>& textures,
    vk::Format format,
    vk::ImageAspectFlags aspectFlags,
    const std::string& name)
{
    VulkanImage result(device, commandPool, textures, name);
    result.GenerateImageView(format, aspectFlags, vk::ImageViewType::eCube, textures.size());
    return std::make_unique<VulkanImage>(std::move(result));
}

std::unique_ptr<VulkanImage>
VulkanImage::CreateImage(
    VulkanDevice& device,
    vk::Format format,
    const vk::Extent2D& swapchainExtent,
    const std::string& name)
{
    VulkanImage result(
        device,
        swapchainExtent.width,
        swapchainExtent.height,
        format,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        name);
    result.GenerateImageView(format, vk::ImageAspectFlagBits::eColor, vk::ImageViewType::e2D);
    return std::make_unique<VulkanImage>(std::move(result));
}

VulkanImage::VulkanImage(
    VulkanDevice& device,
    std::uint32_t width,
    std::uint32_t height,
    vk::Format format,
    vk::ImageTiling tiling,
    vk::ImageUsageFlags usage,
    vk::MemoryPropertyFlags memoryProperty,
    const std::string& name)
    : VulkanEntity(name, device.Handle())
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

    mUniqueImageHolder = Device().createImageUnique(createInfo);
    VulkanEntity::SetHandle(std::move(mUniqueImageHolder.value().get()));

    vk::MemoryRequirements requirements = Device().getImageMemoryRequirements(Handle());
    std::uint32_t memoryType
        = VulkanBuffer::FindMemoryType(device.GetPhysicalDevice(), requirements.memoryTypeBits, memoryProperty);

    auto allocateInfo = vk::MemoryAllocateInfo().setAllocationSize(requirements.size).setMemoryTypeIndex(memoryType);

    mDeviceMemory = Device().allocateMemoryUnique(allocateInfo);
    Device().bindImageMemory(Handle(), mDeviceMemory.get(), 0);
}

VulkanImage::VulkanImage(VulkanDevice& device, vk::Image image, const std::string& name)
    : VulkanEntity(name, device.Handle())
{
    VulkanEntity::SetHandle(std::move(image));
}

void
VulkanImage::Transition(
    VulkanCommandPool& commandPool,
    vk::Format format,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout,
    std::size_t layerCount)
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
                                                        .setLayerCount(static_cast<std::uint32_t>(layerCount))
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
    const Core::Vector2d<std::uint32_t>& size,
    std::size_t layerCount)
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
                                                       .setLayerCount(static_cast<std::uint32_t>(layerCount)))
                              .setImageOffset({ 0, 0, 0 })
                              .setImageExtent({ size.x, size.y, 1 });

            commandBuffer.copyBufferToImage(buffer.Handle(), Handle(), vk::ImageLayout::eTransferDstOptimal, region);
        });
}

const vk::ImageView&
VulkanImage::GetImageView() const
{
    return mImageView.get();
}

void
VulkanImage::GenerateImageView(
    vk::Format format,
    vk::ImageAspectFlags aspectFlags,
    vk::ImageViewType viewType,
    std::size_t layerCount)
{
    auto imageViewCreateInfo = vk::ImageViewCreateInfo()
                                   .setImage(Handle())
                                   .setViewType(viewType)
                                   .setFormat(format)
                                   .setComponents(vk::ComponentMapping {}) // Identity by default
                                   .setSubresourceRange(vk::ImageSubresourceRange {}
                                                            .setAspectMask(aspectFlags)
                                                            .setBaseArrayLayer(0)
                                                            .setBaseMipLevel(0)
                                                            .setLayerCount(static_cast<std::uint32_t>(layerCount))
                                                            .setLevelCount(mMipLevels));

    mImageView = Device().createImageViewUnique(imageViewCreateInfo);
}

void
VulkanImage::GenerateMipmaps(
    VulkanCommandPool& commandPool,
    const Core::Vector2d<std::uint32_t>& size,
    vk::Format format,
    std::size_t layerCount)
{
    (void)format;
    /*if (!mDevice.DoesSupportBlitting(format))
    {
        throw std::runtime_error("Device doesn't support blitting");
    }*/

    commandPool.ExecuteSingleCommand(
        [&, this](vk::CommandBuffer& commandBuffer)
        {
            auto barrier = vk::ImageMemoryBarrier()
                               .setImage(Handle())
                               .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                               .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                               .setSubresourceRange(vk::ImageSubresourceRange()
                                                        .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                                        .setLayerCount(static_cast<std::uint32_t>(layerCount))
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
                auto dstOffsets
                    = std::array<vk::Offset3D, 2> { vk::Offset3D(0, 0, 0),
                                                    vk::Offset3D(
                                                        static_cast<std::int32_t>(mipWidth > 1 ? mipWidth / 2 : 1),
                                                        static_cast<std::int32_t>(mipHeight > 1 ? mipHeight / 2 : 1),
                                                        1) };

                auto blit = vk::ImageBlit()
                                .setSrcOffsets(srcOffsets)
                                .setSrcSubresource(vk::ImageSubresourceLayers()
                                                       .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                                       .setMipLevel(i - 1)
                                                       .setBaseArrayLayer(0)
                                                       .setLayerCount(static_cast<std::uint32_t>(layerCount)))
                                .setDstOffsets(dstOffsets)
                                .setDstSubresource(vk::ImageSubresourceLayers()
                                                       .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                                       .setMipLevel(i)
                                                       .setBaseArrayLayer(0)
                                                       .setLayerCount(static_cast<std::uint32_t>(layerCount)));

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
