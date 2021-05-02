#include <Vulkan/VulkanMesh.h>

namespace Lucid::Vulkan
{

VulkanMesh::VulkanMesh(VulkanDevice& device, VulkanCommandPool& manager, const Core::Mesh& mesh)
	: mVertexBuffer(device, manager, mesh.vertices)
	, mIndexBuffer(device, manager, mesh.indices)
{

}

void VulkanMesh::Draw(vk::UniqueCommandBuffer& commandBuffer, VulkanPipeline& pipeline) const
{
	vk::Buffer vertexBuffers[] = { mVertexBuffer.Handle() };
	vk::DeviceSize offsets[] = { 0 };
	commandBuffer->bindVertexBuffers(0, 1, vertexBuffers, offsets);
	commandBuffer->bindIndexBuffer(mIndexBuffer.Handle(), 0, vk::IndexType::eUint32);
	// commandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.Layout(), 0, 1, &descriptorPool.GetDescriptorSet(i), 0, {});
	commandBuffer->drawIndexed(static_cast<std::uint32_t>(mIndexBuffer.IndicesCount()), 1, 0, 0, 0);
}

}