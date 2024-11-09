#include "VulkanSkybox.h"

#include <Utils/Files.h>

namespace Lucid::Vulkan
{

VulkanSkybox::VulkanSkybox(VulkanDevice& device, VulkanDescriptorPool& pool, VulkanCommandPool& manager)
    : mDescriptorSet(device, pool, "Skybox descriptor set")
    , mUniformBuffer(device, "Skybox uniform buffer")
    , mMaterialBuffer(device, "Skybox material buffer")
{
    Core::MeshPtr mesh = Files::LoadModel("Resources/Models/Cube.obj")->GetOptionalMesh().value();
    mIndexBuffer = std::make_unique<VulkanIndexBuffer>(device, manager, mesh->indices, "Skybox index buffer");
    mVertexBuffer = std::make_unique<VulkanVertexBuffer>(device, manager, mesh->vertices, "Skybox vertex buffer");

    std::array<Core::TexturePtr, 6> textures { Lucid::Files::LoadTexture("Resources/Skyboxes/BACK.jpeg"),
                                               Lucid::Files::LoadTexture("Resources/Skyboxes/FRONT.jpeg"),
                                               Lucid::Files::LoadTexture("Resources/Skyboxes/LEFT.jpeg"),
                                               Lucid::Files::LoadTexture("Resources/Skyboxes/RIGHT.jpeg"),
                                               Lucid::Files::LoadTexture("Resources/Skyboxes/UP.jpeg"),
                                               Lucid::Files::LoadTexture("Resources/Skyboxes/DOWN.jpeg") };

    mTexture = VulkanImage::FromCubemap(
        device, manager, textures, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor, "Skybox cubemap image");

    mSampler = std::make_unique<VulkanSampler>(device, mTexture->GetMipLevels(), "Skybox image sampler");

    Core::MaterialBufferObject materialData;
    materialData.color = glm::vec4 {};
    materialData.metalness = 0.0f;
    materialData.roughness = 0.0f;
    mMaterialBuffer.Write(&materialData);

    auto bufferInfo = vk::DescriptorBufferInfo()
                          .setBuffer(mUniformBuffer.Handle())
                          .setOffset(0)
                          .setRange(sizeof(Core::UniformBufferObject));

    auto imageInfo = vk::DescriptorImageInfo()
                         .setSampler(mSampler->Handle())
                         .setImageView(mTexture->GetImageView())
                         .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    auto imageInfo2 = vk::DescriptorImageInfo()
                          .setSampler(mSampler->Handle())
                          .setImageView(mTexture->GetImageView())
                          .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    auto imageInfo3 = vk::DescriptorImageInfo()
                          .setSampler(mSampler->Handle())
                          .setImageView(mTexture->GetImageView())
                          .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    auto materialInfo = vk::DescriptorBufferInfo()
                            .setBuffer(mMaterialBuffer.Handle())
                            .setOffset(0)
                            .setRange(sizeof(Core::MaterialBufferObject));

    mDescriptorSet.Update(bufferInfo, imageInfo, imageInfo2, imageInfo3, materialInfo);
}

void
VulkanSkybox::Draw(vk::CommandBuffer& commandBuffer, VulkanPipeline& pipeline) const
{
    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, pipeline.Layout(), 0, 1, &mDescriptorSet.Handle(), 0, {});

    vk::Buffer vertexBuffers[] = { mVertexBuffer->Handle() };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
    commandBuffer.bindIndexBuffer(mIndexBuffer->Handle(), 0, vk::IndexType::eUint32);
    commandBuffer.drawIndexed(static_cast<std::uint32_t>(mIndexBuffer->IndicesCount()), 1, 0, 0, 0);
}

void
VulkanSkybox::UpdateTransform(const Core::UniformBufferObject& ubo)
{
    mUniformBuffer.Write(const_cast<Core::UniformBufferObject*>(&ubo));
}

} // namespace Lucid::Vulkan
