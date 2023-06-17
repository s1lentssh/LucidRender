#include <Core/UniformBufferObject.h>
#include <Utils/Files.h>
#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanMesh.h>

namespace Lucid::Vulkan
{

VulkanMesh::VulkanMesh(
    VulkanDevice& device,
    VulkanDescriptorPool& pool,
    VulkanCommandPool& manager,
    const Core::MeshPtr& mesh)
    : mVertexBuffer(device, manager, mesh->vertices)
    , mIndexBuffer(device, manager, mesh->indices)
    , mUniformBuffer(device)
{
    static auto DefaultTexture = Lucid::Files::LoadTexture("Resources/Textures/Default.png");

    mTexture = VulkanImage::FromTexture(
        device,
        manager,
        mesh->texture == nullptr ? DefaultTexture : mesh->texture,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageAspectFlagBits::eColor);

    mSampler = std::make_unique<VulkanSampler>(device, mTexture->GetMipLevels());
    mDescriptorSet = std::make_unique<VulkanDescriptorSet>(device, pool);

    auto bufferInfo = vk::DescriptorBufferInfo()
                          .setBuffer(mUniformBuffer.Handle().get())
                          .setOffset(0)
                          .setRange(sizeof(Core::UniformBufferObject));

    auto imageInfo = vk::DescriptorImageInfo()
                         .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                         .setImageView(mTexture->GetImageView())
                         .setSampler(mSampler->Handle().get());

    mDescriptorSet->Update(bufferInfo, imageInfo);
}

void
VulkanMesh::Draw(vk::CommandBuffer& commandBuffer, VulkanPipeline& pipeline) const
{
    vk::Buffer vertexBuffers[] = { mVertexBuffer.Handle().get() };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
    commandBuffer.bindIndexBuffer(mIndexBuffer.Handle().get(), 0, vk::IndexType::eUint32);
    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, pipeline.Layout(), 0, 1, &mDescriptorSet->Handle().get(), 0, {});
    commandBuffer.drawIndexed(static_cast<std::uint32_t>(mIndexBuffer.IndicesCount()), 1, 0, 0, 0);
}

void
VulkanMesh::UpdateTransform(const Core::UniformBufferObject& ubo)
{
    mUniformBuffer.Write(const_cast<Core::UniformBufferObject*>(&ubo));
}

} // namespace Lucid::Vulkan
