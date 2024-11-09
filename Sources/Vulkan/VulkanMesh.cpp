#include <Core/UniformBufferObject.h>
#include <Utils/Files.h>
#include <Utils/Logger.hpp>
#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanMesh.h>

namespace Lucid::Vulkan
{

VulkanMesh::VulkanMesh(
    VulkanDevice& device,
    VulkanDescriptorPool& pool,
    VulkanCommandPool& manager,
    const Core::MeshPtr& mesh)
    : mVertexBuffer(device, manager, mesh->vertices, "Vertex buffer of" + mesh->name)
    , mIndexBuffer(device, manager, mesh->indices, "Index buffer of" + mesh->name)
    , mUniformBuffer(device, "Unifrom buffer of" + mesh->name)
    , mMaterialBuffer(device, "Material buffer of" + mesh->name)
{
    static auto DefaultTexture = Lucid::Files::LoadTexture("Resources/Textures/Default.png");

    // Fill material buffer
    Core::MaterialBufferObject materialData;
    materialData.color = glm::vec4(1.0);
    materialData.metalness = 0.0f;
    materialData.roughness = 0.0f;
    mMaterialBuffer.Write(&materialData);

    // Classic setup
    Core::TexturePtr albedo;
    if (mesh->material != nullptr && mesh->material->albedo != nullptr)
    {
        albedo = mesh->material->albedo;
    }
    else
    {
        albedo = std::make_shared<Core::Texture>();
        albedo->mipLevels = 1;
        albedo->size = { 1, 1 };
        albedo->pixels = { static_cast<std::uint8_t>(mesh->material->baseColorFactor.r * 255),
                           static_cast<std::uint8_t>(mesh->material->baseColorFactor.g * 255),
                           static_cast<std::uint8_t>(mesh->material->baseColorFactor.b * 255),
                           static_cast<std::uint8_t>(mesh->material->baseColorFactor.a * 255) };
    }

    Core::TexturePtr metallicRoughness;
    if (mesh->material != nullptr && mesh->material->metallicRoughness != nullptr)
    {
        metallicRoughness = mesh->material->metallicRoughness;
    }
    else
    {
        metallicRoughness = std::make_shared<Core::Texture>();
        metallicRoughness->mipLevels = 1;
        metallicRoughness->size = { 1, 1 };
        metallicRoughness->pixels = { 0,
                                      static_cast<std::uint8_t>(mesh->material->roughnessFactor * 255),
                                      static_cast<std::uint8_t>(mesh->material->metalnessFactor * 255),
                                      0 };
    }

    Core::TexturePtr normal;
    if (mesh->material != nullptr && mesh->material->normal != nullptr)
    {
        normal = mesh->material->normal;
    }
    else
    {
        normal = std::make_shared<Core::Texture>();
        normal->mipLevels = 1;
        normal->size = { 1, 1 };
        normal->pixels = { 0, 0, 0, 0 };
    }

    mAlbedoTexture = VulkanImage::FromTexture(
        device,
        manager,
        albedo,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageAspectFlagBits::eColor,
        "Albedo texture of " + mesh->name);

    mMetallicRoughnessTexture = VulkanImage::FromTexture(
        device,
        manager,
        metallicRoughness,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageAspectFlagBits::eColor,
        "Metallic+Roughness texture of " + mesh->name);

    mNormalTexture = VulkanImage::FromTexture(
        device,
        manager,
        normal,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageAspectFlagBits::eColor,
        "Normal texture of " + mesh->name);

    mAlbedoSampler
        = std::make_unique<VulkanSampler>(device, mAlbedoTexture->GetMipLevels(), "Albedo Sampler of " + mesh->name);
    mMetallicRoughnessSampler = std::make_unique<VulkanSampler>(
        device, mMetallicRoughnessTexture->GetMipLevels(), "Metallic+Roughness Sampler of " + mesh->name);
    mNormalSampler
        = std::make_unique<VulkanSampler>(device, mNormalTexture->GetMipLevels(), "Normal Sampler of " + mesh->name);

    mDescriptorSet = std::make_unique<VulkanDescriptorSet>(device, pool, "Descriptor Set of " + mesh->name);

    auto bufferInfo = vk::DescriptorBufferInfo()
                          .setBuffer(mUniformBuffer.Handle())
                          .setOffset(0)
                          .setRange(sizeof(Core::UniformBufferObject));

    auto albedoImageInfo = vk::DescriptorImageInfo()
                               .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                               .setImageView(mAlbedoTexture->GetImageView())
                               .setSampler(mAlbedoSampler->Handle());

    auto metallicRoughnessImageInfo = vk::DescriptorImageInfo()
                                          .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                                          .setImageView(mMetallicRoughnessTexture->GetImageView())
                                          .setSampler(mMetallicRoughnessSampler->Handle());

    auto normalImageInfo = vk::DescriptorImageInfo()
                               .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                               .setImageView(mNormalTexture->GetImageView())
                               .setSampler(mNormalSampler->Handle());

    auto materialInfo = vk::DescriptorBufferInfo()
                            .setBuffer(mMaterialBuffer.Handle())
                            .setOffset(0)
                            .setRange(sizeof(Core::MaterialBufferObject));

    mDescriptorSet->Update(bufferInfo, albedoImageInfo, metallicRoughnessImageInfo, normalImageInfo, materialInfo);
}

void
VulkanMesh::Draw(vk::CommandBuffer& commandBuffer, VulkanPipeline& pipeline) const
{
    vk::Buffer vertexBuffers[] = { mVertexBuffer.Handle() };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
    commandBuffer.bindIndexBuffer(mIndexBuffer.Handle(), 0, vk::IndexType::eUint32);
    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, pipeline.Layout(), 0, 1, &mDescriptorSet->Handle(), 0, {});
    commandBuffer.drawIndexed(static_cast<std::uint32_t>(mIndexBuffer.IndicesCount()), 1, 0, 0, 0);
}

void
VulkanMesh::UpdateTransform(const Core::UniformBufferObject& ubo)
{
    mUniformBuffer.Write(const_cast<Core::UniformBufferObject*>(&ubo));
}

} // namespace Lucid::Vulkan
