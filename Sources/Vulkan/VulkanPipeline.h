#pragma once

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
    VulkanPipeline(
        VulkanDevice& device,
        const vk::Extent2D& extent,
        VulkanRenderPass& renderPass,
        VulkanDescriptorPool& descriptorPool);

    [[nodiscard]] const vk::PipelineLayout& Layout() const;

private:
    [[nodiscard]] static std::array<vk::VertexInputBindingDescription, 1> GetBindingDescriptions();
    [[nodiscard]] static std::array<vk::VertexInputAttributeDescription, 4> GetAttributeDescriptions();

    vk::UniquePipelineLayout mLayout;
};

} // namespace Lucid::Vulkan
