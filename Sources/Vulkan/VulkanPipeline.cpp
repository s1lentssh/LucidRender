#include "VulkanPipeline.h"

#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanSwapchain.h>
#include <Vulkan/VulkanRenderPass.h>
#include <Vulkan/VulkanShader.h>
#include <Utils/Logger.hpp>

namespace Lucid
{

VulkanPipeline::VulkanPipeline(VulkanDevice& device, VulkanSwapchain& swapchain, VulkanRenderPass& renderPass)
	: mDevice(device)
	, mSwapchain(swapchain)
	, mRenderPass(renderPass)
{
	Init();
	CreateFramebuffers();
}

void VulkanPipeline::Init()
{
	VulkanShader vertexShader(mDevice, "Resources/Shaders/Vert.spv");
	VulkanShader fragmentShader(mDevice, "Resources/Shaders/Frag.spv");

	auto vertexShaderStageInfo = vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eVertex)
		.setModule(vertexShader.Handle().get())
		.setPName("main");

	auto fragmentShaderStageInfo = vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eFragment)
		.setModule(fragmentShader.Handle().get())
		.setPName("main");

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

	auto vertexInputState = vk::PipelineVertexInputStateCreateInfo()
		.setVertexBindingDescriptionCount(0)
		.setVertexAttributeDescriptionCount(0);

	auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo()
		.setTopology(vk::PrimitiveTopology::eTriangleList)
		.setPrimitiveRestartEnable(false);

	vk::Extent2D extent = mSwapchain.GetExtent();

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
		.setFrontFace(vk::FrontFace::eClockwise)
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
		.setSetLayoutCount(0)
		.setPushConstantRangeCount(0);

	mLayout = mDevice.Handle()->createPipelineLayoutUnique(pipelineLayoutCreateInfo);

	auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
		.setStageCount(static_cast<std::uint32_t>(std::size(shaderStages)))
		.setPStages(shaderStages)
		.setPVertexInputState(&vertexInputState)
		.setPInputAssemblyState(&inputAssemblyState)
		.setPViewportState(&viewportState)
		.setPRasterizationState(&rasterizationState)
		.setPMultisampleState(&multisampleState)
		.setPColorBlendState(&colorBlendState)
		.setLayout(mLayout.get())
		.setRenderPass(mRenderPass.Handle().get())
		.setSubpass(0);

	mPipeline = mDevice.Handle()->createGraphicsPipelineUnique({}, pipelineCreateInfo);
	Logger::Info("Pipeline created");
}

void VulkanPipeline::CreateFramebuffers()
{
	const std::vector<vk::UniqueImageView>& imageViews = mSwapchain.GetImageViews();
	const vk::Extent2D extent = mSwapchain.GetExtent();

	mFramebuffers.reserve(imageViews.size());

	for (const auto& imageView : imageViews)
	{
		auto framebufferCreateInfo = vk::FramebufferCreateInfo()
			.setRenderPass(mRenderPass.Handle().get())
			.setAttachmentCount(1)
			.setPAttachments(&imageView.get())
			.setWidth(extent.width)
			.setHeight(extent.height)
			.setLayers(1);

		mFramebuffers.push_back(mDevice.Handle()->createFramebufferUnique(framebufferCreateInfo));
	}

	Logger::Info("Framebuffers created");
}

}
