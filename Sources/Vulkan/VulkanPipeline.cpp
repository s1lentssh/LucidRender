#include "VulkanPipeline.h"

#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanSwapchain.h>
#include <Vulkan/VulkanRenderPass.h>
#include <Vulkan/VulkanShader.h>
#include <Vulkan/VulkanVertex.h>
#include <Vulkan/VulkanDescriptorPool.h>
#include <Utils/Logger.hpp>

namespace Lucid::Vulkan
{

VulkanPipeline::VulkanPipeline(
	VulkanDevice& device, 
	const vk::Extent2D& extent, 
	VulkanRenderPass& renderPass, 
	VulkanDescriptorPool& descriptorPool)
{
	VulkanShader vertexShader(device, VulkanShader::Type::Vertex, "Resources/Shaders/Shader.vert");
	VulkanShader fragmentShader(device, VulkanShader::Type::Fragment, "Resources/Shaders/Shader.frag");

	auto vertexShaderStageInfo = vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eVertex)
		.setModule(vertexShader.Handle())
		.setPName("main");

	auto fragmentShaderStageInfo = vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eFragment)
		.setModule(fragmentShader.Handle())
		.setPName("main");

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

	auto vertexBindingDescriptions = VulkanVertex::GetBindingDescriptions();
	auto vertexAttributeDescriptions = VulkanVertex::GetAttributeDescriptions();

	auto vertexInputState = vk::PipelineVertexInputStateCreateInfo()
		.setVertexBindingDescriptionCount(static_cast<std::uint32_t>(vertexBindingDescriptions.size()))
		.setPVertexBindingDescriptions(vertexBindingDescriptions.data())
		.setVertexAttributeDescriptionCount(static_cast<std::uint32_t>(vertexAttributeDescriptions.size()))
		.setPVertexAttributeDescriptions(vertexAttributeDescriptions.data());

	auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo()
		.setTopology(vk::PrimitiveTopology::eTriangleList)
		.setPrimitiveRestartEnable(false);

	auto viewport = vk::Viewport()
		.setX(0.0f)
		.setY(0.0f)
		.setMinDepth(0.0f)
		.setMaxDepth(1.0f)
		.setWidth(static_cast<float>(extent.width))
		.setHeight(static_cast<float>(extent.height));

	auto scissor = vk::Rect2D()
		.setExtent(extent)
		.setOffset({ 0, 0 });

	auto viewportState = vk::PipelineViewportStateCreateInfo()
		.setViewportCount(1)
		.setPViewports(&viewport)
		.setScissorCount(1)
		.setPScissors(&scissor);

	auto rasterizationState = vk::PipelineRasterizationStateCreateInfo()
		.setDepthClampEnable(false)
		.setRasterizerDiscardEnable(false)
		.setPolygonMode(vk::PolygonMode::eFill)
		.setLineWidth(1.0f)
		.setCullMode(vk::CullModeFlagBits::eBack)
		.setFrontFace(vk::FrontFace::eCounterClockwise)
		.setDepthBiasEnable(false);

	auto multisampleState = vk::PipelineMultisampleStateCreateInfo()
		.setSampleShadingEnable(false)
		.setRasterizationSamples(vk::SampleCountFlagBits::e1);

	auto colorBlendAttachmentState = vk::PipelineColorBlendAttachmentState()
		.setColorWriteMask(
			vk::ColorComponentFlagBits::eR |
			vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA)
		.setBlendEnable(false);

	auto colorBlendState = vk::PipelineColorBlendStateCreateInfo()
		.setLogicOpEnable(false)
		.setLogicOp(vk::LogicOp::eCopy)
		.setAttachmentCount(1)
		.setPAttachments(&colorBlendAttachmentState)
		.setBlendConstants({ 0.0f, 0.0f, 0.0f, 0.0f });

	auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
		.setPushConstantRangeCount(0)
		.setSetLayoutCount(1)
		.setPSetLayouts(&descriptorPool.Layout());

	mLayout = device.Handle().createPipelineLayoutUnique(pipelineLayoutCreateInfo);

	auto depthStencilState = vk::PipelineDepthStencilStateCreateInfo()
		.setDepthTestEnable(true)
		.setDepthWriteEnable(true)
		.setDepthCompareOp(vk::CompareOp::eLess)
		.setDepthBoundsTestEnable(false)
		.setStencilTestEnable(false);

	auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
		.setStageCount(static_cast<std::uint32_t>(std::size(shaderStages)))
		.setPStages(shaderStages)
		.setPVertexInputState(&vertexInputState)
		.setPInputAssemblyState(&inputAssemblyState)
		.setPViewportState(&viewportState)
		.setPRasterizationState(&rasterizationState)
		.setPMultisampleState(&multisampleState)
		.setPColorBlendState(&colorBlendState)
		.setPDepthStencilState(&depthStencilState)
		.setLayout(mLayout.get())
		.setRenderPass(renderPass.Handle())
		.setSubpass(0);

	mHandle = device.Handle().createGraphicsPipelineUnique({}, pipelineCreateInfo);
}

const vk::PipelineLayout& VulkanPipeline::Layout() const 
{ 
	return mLayout.get(); 
}

}
