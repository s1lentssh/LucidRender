#include "VulkanSkybox.h"

#include <Utils/Files.h>

namespace Lucid::Vulkan
{

VulkanSkybox::VulkanSkybox(VulkanDevice& device, VulkanDescriptorPool& pool, VulkanCommandPool& manager)
    : mDescriptorSet(device, pool)
    , mUniformBuffer(device)
{
    Core::MeshPtr mesh = Files::LoadModel("Resources/Models/Cube.obj")->GetOptionalMesh().value();
    mIndexBuffer = std::make_unique<VulkanIndexBuffer>(device, manager, mesh->indices);
    mVertexBuffer = std::make_unique<VulkanVertexBuffer>(device, manager, mesh->vertices);

    std::array<Core::TexturePtr, 6> textures { Lucid::Files::LoadTexture("Resources/Skyboxes/BACK.jpeg"),
                                               Lucid::Files::LoadTexture("Resources/Skyboxes/FRONT.jpeg"),
                                               Lucid::Files::LoadTexture("Resources/Skyboxes/LEFT.jpeg"),
                                               Lucid::Files::LoadTexture("Resources/Skyboxes/RIGHT.jpeg"),
                                               Lucid::Files::LoadTexture("Resources/Skyboxes/UP.jpeg"),
                                               Lucid::Files::LoadTexture("Resources/Skyboxes/DOWN.jpeg") };

    mTexture = VulkanImage::FromCubemap(
        device, manager, textures, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);

    mSampler = std::make_unique<VulkanSampler>(device, mTexture->GetMipLevels());

    auto bufferInfo = vk::DescriptorBufferInfo()
                          .setBuffer(mUniformBuffer.Handle().get())
                          .setOffset(0)
                          .setRange(sizeof(Core::UniformBufferObject));

    auto imageInfo = vk::DescriptorImageInfo()
                         .setSampler(mSampler->Handle().get())
                         .setImageView(mTexture->GetImageView())
                         .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    mDescriptorSet.Update(bufferInfo, imageInfo);
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
