#include "VulkanSkybox.h"

#include <Utils/Files.h>

namespace Lucid::Vulkan
{

VulkanSkybox::VulkanSkybox(
    VulkanDevice& device,
    VulkanDescriptorPool& pool,
    VulkanCommandPool& manager,
    const std::array<Core::Texture, 6>& textures)
    : mTexture(
        VulkanImage::FromCubemap(device, manager, textures, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor))
    , mSampler(device, mTexture->GetMipLevels())
    , mDescriptorSet(device, pool)
    , mUniformBuffer(device)
{
    auto bufferInfo = vk::DescriptorBufferInfo()
                          .setBuffer(mUniformBuffer.Handle().get())
                          .setOffset(0)
                          .setRange(sizeof(Core::UniformBufferObject));

    auto imageInfo = vk::DescriptorImageInfo()
                         .setSampler(mSampler.Handle().get())
                         .setImageView(mTexture->GetImageView())
                         .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    mDescriptorSet.Update(bufferInfo, imageInfo);

    auto cube = Files::LoadModel("Resources/Models/Cube.obj");
    mIndexBuffer = std::make_unique<VulkanIndexBuffer>(device, manager, cube.indices);
    mVertexBuffer = std::make_unique<VulkanVertexBuffer>(device, manager, cube.vertices);
}

void
VulkanSkybox::Draw(vk::CommandBuffer& commandBuffer, VulkanPipeline& pipeline) const
{
    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, pipeline.Layout(), 0, 1, &mDescriptorSet.Handle().get(), 0, {});

    vk::Buffer vertexBuffers[] = { mVertexBuffer->Handle().get() };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
    commandBuffer.bindIndexBuffer(mIndexBuffer->Handle().get(), 0, vk::IndexType::eUint32);
    commandBuffer.drawIndexed(static_cast<std::uint32_t>(mIndexBuffer->IndicesCount()), 1, 0, 0, 0);
}

void
VulkanSkybox::UpdateTransform(const Core::UniformBufferObject& ubo)
{
    mUniformBuffer.Write(const_cast<Core::UniformBufferObject*>(&ubo));
}

} // namespace Lucid::Vulkan
