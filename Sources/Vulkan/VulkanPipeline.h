#pragma once

#include <memory>

#include <Vulkan/VulkanEntity.h>
#include <vulkan/vulkan.hpp>

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanSwapchain;
class VulkanRenderPass;
class VulkanDescriptorPool;

class VulkanPipeline : public VulkanEntity<vk::UniquePipeline>
{
public:
    static std::unique_ptr<VulkanPipeline> Default(
        VulkanDevice& device,
        const vk::Extent2D& extent,
        VulkanRenderPass& renderPass,
        VulkanDescriptorPool& descriptorPool);

    static std::unique_ptr<VulkanPipeline> Skybox(
        VulkanDevice& device,
        const vk::Extent2D& extent,
        VulkanRenderPass& renderPass,
        VulkanDescriptorPool& descriptorPool);

    [[nodiscard]] const vk::PipelineLayout& Layout() const;

    VulkanPipeline(
        VulkanDevice& device,
        const vk::Extent2D& extent,
        VulkanRenderPass& renderPass,
        VulkanDescriptorPool& descriptorPool,
        const std::string& shaderName,
        bool depthWriteTest,
        vk::CullModeFlagBits cullMode);

private:
    [[nodiscard]] static std::array<vk::VertexInputBindingDescription, 1> GetBindingDescriptions();
    [[nodiscard]] static std::array<vk::VertexInputAttributeDescription, 4> GetAttributeDescriptions();

    vk::UniquePipelineLayout mLayout;
};

} // namespace Lucid::Vulkan
