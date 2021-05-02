#pragma once

#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanPipeline.h>
#include <Core/Mesh.h>

namespace Lucid::Vulkan
{

class VulkanMesh
{
public:
	VulkanMesh(VulkanDevice& device, VulkanCommandPool& manager, const Core::Mesh& mesh);
	void Draw(vk::UniqueCommandBuffer& commandBuffer, VulkanPipeline& pipeline) const;

private:
	VulkanVertexBuffer mVertexBuffer;
	VulkanIndexBuffer mIndexBuffer;
	vk::PipelineLayout mPipelineLayout;
};

}