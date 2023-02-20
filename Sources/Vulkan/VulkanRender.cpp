#include "VulkanRender.h"

#include <Utils/Defaults.hpp>
#include <Utils/Logger.hpp>
#include <Utils/Files.h>
#include <Core/UniformBufferObject.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Lucid::Vulkan
{

VulkanRender::VulkanRender(const Core::IWindow& window, const Core::Scene& scene) : mWindow(&window), mScene(scene)
{
    // Create instance
    mInstance = std::make_unique<VulkanInstance>(window.GetRequiredInstanceExtensions());

    // Create surface
    mSurface = std::make_unique<VulkanSurface>(*mInstance.get(), window);

    // Create and init device
    mDevice = std::make_unique<VulkanDevice>(mInstance->PickSuitableDeviceForSurface(*mSurface.get()));
    mDevice->InitLogicalDeviceForSurface(*mSurface.get());

    // Create descriptor pool
    mDescriptorPool = std::make_unique<VulkanDescriptorPool>(*mDevice.get());

    // Create command pool
    mCommandPool = std::make_unique<VulkanCommandPool>(*mDevice.get());

    RecreateSwapchain();

    // Create semaphores
    auto semaphoreCreateInfo = vk::SemaphoreCreateInfo();

    auto fenceCreateInfo = vk::FenceCreateInfo()
        .setFlags(vk::FenceCreateFlagBits::eSignaled);
    
    for (std::uint32_t i = 0; i < Defaults::MaxFramesInFlight; i++)
    {
        mImagePresentedSemaphores.push_back(mDevice->Handle()->createSemaphoreUnique(semaphoreCreateInfo));
        mRenderFinishedSemaphores.push_back(mDevice->Handle()->createSemaphoreUnique(semaphoreCreateInfo));
        mInFlightFences.push_back(mDevice->Handle()->createFenceUnique(fenceCreateInfo));
    }

    mImagesInFlight.resize(Defaults::MaxFramesInFlight, {});
}

VulkanRender::~VulkanRender()
{
    mDevice->Handle()->waitIdle();
}

void VulkanRender::DrawFrame()
{
    mDevice->Handle()->waitIdle();

    mCommandPool->RecordCommandBuffers(*mSwapchain.get(), *mRenderPass.get(), [this](vk::CommandBuffer& commandBuffer)
    {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline->Handle().get());

        static Core::PushConstants constants;
        constants.ambientColor = glm::make_vec3(Defaults::AmbientColor.data());
        constants.ambientFactor = 3.0f;
        constants.lightPosition = glm::vec3(10.0, 50.0, 0.0);
        constants.lightColor = glm::vec3(10.0, 10.0, 10.0);
        commandBuffer.pushConstants(mPipeline->Layout(), vk::ShaderStageFlagBits::eFragment, 0, sizeof(Core::PushConstants), &constants);

        for (const VulkanMesh& mesh : mMeshes)
        {
            mesh.Draw(commandBuffer, *mPipeline.get());
        }
    });
    
    // Render frame
    auto result = mDevice->Handle()->waitForFences(mInFlightFences[mCurrentFrame].get(), true, std::numeric_limits<std::uint64_t>::max());
    (void) result;

    vk::ResultValue acquireResult = mSwapchain->AcquireNextImage(mImagePresentedSemaphores[mCurrentFrame]);

    if (vk::Result::eErrorOutOfDateKHR == acquireResult.result)
    {
        RecreateSwapchain();
    }
    else if (vk::Result::eSuccess != acquireResult.result && vk::Result::eSuboptimalKHR != acquireResult.result)
    {
        throw std::runtime_error("Error during render");
    }

    std::uint32_t imageIndex = acquireResult.value;

    UpdateUniformBuffers();

    // Fix if max frames in flight greater than swapchain image count or if aquire returns out of order
    if (mImagesInFlight[imageIndex])
    {
        auto result2 = mDevice->Handle()->waitForFences(mImagesInFlight[imageIndex], true, std::numeric_limits<std::uint64_t>::max());
        (void) result2;
    }

    mImagesInFlight[imageIndex] = mInFlightFences[mCurrentFrame].get();

    vk::Semaphore waitSemaphores[] = { mImagePresentedSemaphores[mCurrentFrame].get() };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    vk::Semaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame].get() };

    auto submitInfo = vk::SubmitInfo()
        .setWaitSemaphoreCount(static_cast<std::uint32_t>(std::size(waitSemaphores)))
        .setPWaitSemaphores(waitSemaphores)
        .setPWaitDstStageMask(waitStages)
        .setCommandBufferCount(1)
        .setPCommandBuffers(&mCommandPool->Get(imageIndex).get())
        .setSignalSemaphoreCount(static_cast<std::uint32_t>(std::size(signalSemaphores)))
        .setPSignalSemaphores(signalSemaphores);

    mDevice->Handle()->resetFences(mInFlightFences[mCurrentFrame].get());
    mDevice->GetGraphicsQueue().submit(submitInfo, mInFlightFences[mCurrentFrame].get());

    // Present frame
    vk::SwapchainKHR swapchains[] = { mSwapchain->Handle().get() };

    auto presentInfo = vk::PresentInfoKHR()
        .setSwapchainCount(static_cast<std::uint32_t>(std::size(swapchains)))
        .setPSwapchains(swapchains)
        .setPImageIndices(&imageIndex)
        .setWaitSemaphoreCount(static_cast<std::uint32_t>(std::size(signalSemaphores)))
        .setPWaitSemaphores(signalSemaphores);

    vk::Result presentResult = mDevice->GetPresentQueue().presentKHR(presentInfo);

    if (vk::Result::eErrorOutOfDateKHR == presentResult || vk::Result::eSuboptimalKHR == presentResult)
    {
        RecreateSwapchain();
    }
    else if (vk::Result::eSuccess != presentResult)
    {
        throw std::runtime_error("Error during present");
    }

    mCurrentFrame = (mCurrentFrame + 1) % Defaults::MaxFramesInFlight;
}

void VulkanRender::AddAsset(const Core::Asset& asset)
{
    mMeshes.push_back(VulkanMesh(*mDevice.get(), *mDescriptorPool.get(), *mCommandPool.get(), asset.GetTexture(), asset.GetMesh()));
    RecordCommandBuffers();
}

void VulkanRender::RecreateSwapchain()
{
    LoggerInfo << "Swapchain recreation";

    mDevice->Handle()->waitIdle();

    while (mWindow->GetSize().x == 0 || mWindow->GetSize().y == 0)
    {
        mWindow->WaitEvents();
    }

    // Create swapchain
    mSwapchain = std::make_unique<VulkanSwapchain>(*mDevice.get(), *mSurface.get(), mWindow->GetSize());

    // Create render pass
    mRenderPass = std::make_unique<VulkanRenderPass>(*mDevice.get(), mSwapchain->GetImageFormat());

    // Create pipeline
    mPipeline = std::make_unique<VulkanPipeline>(*mDevice.get(), mSwapchain->GetExtent(), *mRenderPass.get(), *mDescriptorPool.get());

    // Create depth image
    mDepthImage = VulkanImage::CreateDepthImage(*mDevice.get(), mSwapchain->GetExtent(), mDevice->FindSupportedDepthFormat(), vk::ImageAspectFlagBits::eDepth);

    // Create MSAA RenderTarget
    mResolveImage = VulkanImage::CreateImage(*mDevice.get(), mSwapchain->GetImageFormat(), mSwapchain->GetExtent());

    // Create framebuffers for swapchain
    mSwapchain->CreateFramebuffers(*mRenderPass.get(), *mDepthImage.get(), *mResolveImage.get());

    RecordCommandBuffers();
}

void VulkanRender::UpdateUniformBuffers()
{
    vk::Extent2D extent = mSwapchain->GetExtent();
    float aspectRatio = extent.width / (float)extent.height;

    Core::UniformBufferObject ubo;
    ubo.view = mScene.GetCamera()->Transform();
    ubo.projection = glm::perspective(glm::radians(mScene.GetCamera()->FieldOfView()), aspectRatio, 0.1f, 120.0f);
    ubo.projection[1][1] *= -1;

    for (std::size_t i = 0; i < mMeshes.size(); i++)
    {
        ubo.model = mScene.GetAssets().at(i).Transform();
        mMeshes.at(i).UpdateTransform(ubo);
    }
}

void VulkanRender::RecordCommandBuffers()
{
    mCommandPool->RecreateCommandBuffers(*mSwapchain.get());
    //mCommandPool->RecordCommandBuffers(*mPipeline.get(), *mSwapchain.get(), *mRenderPass.get(), mMeshes, [](auto& data){});
}

}
