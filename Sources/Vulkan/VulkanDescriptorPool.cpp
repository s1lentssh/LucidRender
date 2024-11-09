#include "VulkanDescriptorPool.h"

#include <Core/UniformBufferObject.h>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanImage.h>
#include <Vulkan/VulkanSampler.h>

namespace Lucid::Vulkan
{

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice& device, const std::string& name)
    : VulkanEntity(name, device.Handle())
{
    const std::uint32_t kMaxDescriptorCount = 1000;
    const std::uint32_t kMaxDescriptorSets = 1000;

    auto samplerPoolSize
        = vk::DescriptorPoolSize().setDescriptorCount(kMaxDescriptorCount).setType(vk::DescriptorType::eSampler);

    auto combinedImageSamplerPoolSize = vk::DescriptorPoolSize()
                                            .setDescriptorCount(kMaxDescriptorCount)
                                            .setType(vk::DescriptorType::eCombinedImageSampler);

    auto sampledImagePoolSize
        = vk::DescriptorPoolSize().setDescriptorCount(kMaxDescriptorCount).setType(vk::DescriptorType::eSampledImage);

    auto storageImagePoolSize
        = vk::DescriptorPoolSize().setDescriptorCount(kMaxDescriptorCount).setType(vk::DescriptorType::eStorageImage);

    auto uniformTexelPoolSize = vk::DescriptorPoolSize()
                                    .setDescriptorCount(kMaxDescriptorCount)
                                    .setType(vk::DescriptorType::eUniformTexelBuffer);

    auto uniformStoragePoolSize = vk::DescriptorPoolSize()
                                      .setDescriptorCount(kMaxDescriptorCount)
                                      .setType(vk::DescriptorType::eStorageTexelBuffer);

    auto uniformPoolSize
        = vk::DescriptorPoolSize().setDescriptorCount(kMaxDescriptorCount).setType(vk::DescriptorType::eUniformBuffer);

    auto storagePoolSize
        = vk::DescriptorPoolSize().setDescriptorCount(kMaxDescriptorCount).setType(vk::DescriptorType::eStorageBuffer);

    auto uniformDynamicPoolSize = vk::DescriptorPoolSize()
                                      .setDescriptorCount(kMaxDescriptorCount)
                                      .setType(vk::DescriptorType::eUniformBufferDynamic);

    auto storageDynamicPoolSize = vk::DescriptorPoolSize()
                                      .setDescriptorCount(kMaxDescriptorCount)
                                      .setType(vk::DescriptorType::eStorageBufferDynamic);

    auto inputAttachmentPoolSize = vk::DescriptorPoolSize()
                                       .setDescriptorCount(kMaxDescriptorCount)
                                       .setType(vk::DescriptorType::eInputAttachment);

    vk::DescriptorPoolSize poolSizes[]
        = { samplerPoolSize,        combinedImageSamplerPoolSize, sampledImagePoolSize,   storageImagePoolSize,
            uniformTexelPoolSize,   uniformStoragePoolSize,       uniformPoolSize,        storagePoolSize,
            uniformDynamicPoolSize, storageDynamicPoolSize,       inputAttachmentPoolSize };

    auto createInfo = vk::DescriptorPoolCreateInfo()
                          .setPoolSizeCount(static_cast<std::uint32_t>(std::size(poolSizes)))
                          .setPPoolSizes(poolSizes)
                          .setMaxSets(kMaxDescriptorSets)
                          .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

    VulkanEntity::SetHandle(device.Handle().createDescriptorPoolUnique(createInfo));

    CreateDescriptorSetLayout();
}

void
VulkanDescriptorPool::CreateDescriptorSetLayout()
{
    auto vertexLayoutBinding = vk::DescriptorSetLayoutBinding()
                                   .setBinding(0)
                                   .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                                   .setDescriptorCount(1)
                                   .setStageFlags(vk::ShaderStageFlagBits::eVertex);

    auto albedoSamplerLayoutBinding = vk::DescriptorSetLayoutBinding()
                                          .setBinding(1)
                                          .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                          .setDescriptorCount(1)
                                          .setStageFlags(vk::ShaderStageFlagBits::eFragment);

    auto metallicRoughnessSamplerLayoutBinding = vk::DescriptorSetLayoutBinding()
                                                     .setBinding(2)
                                                     .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                                     .setDescriptorCount(1)
                                                     .setStageFlags(vk::ShaderStageFlagBits::eFragment);

    auto normalSamplerLayoutBinding = vk::DescriptorSetLayoutBinding()
                                          .setBinding(3)
                                          .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                          .setDescriptorCount(1)
                                          .setStageFlags(vk::ShaderStageFlagBits::eFragment);

    auto materialLayoutBinding = vk::DescriptorSetLayoutBinding()
                                     .setBinding(4)
                                     .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                                     .setDescriptorCount(1)
                                     .setStageFlags(vk::ShaderStageFlagBits::eFragment);

    vk::DescriptorSetLayoutBinding bindings[] = { vertexLayoutBinding,
                                                  albedoSamplerLayoutBinding,
                                                  metallicRoughnessSamplerLayoutBinding,
                                                  normalSamplerLayoutBinding,
                                                  materialLayoutBinding };

    auto createInfo = vk::DescriptorSetLayoutCreateInfo()
                          .setBindingCount(static_cast<std::uint32_t>(std::size(bindings)))
                          .setPBindings(bindings);

    mDescriptorSetLayout = Device().createDescriptorSetLayoutUnique(createInfo);
}

} // namespace Lucid::Vulkan
