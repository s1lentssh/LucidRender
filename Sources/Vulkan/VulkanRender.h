#pragma once

#include <Core/Interfaces.h>
#include <Core/Scene.h>
#include <Utils/Defaults.hpp>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanCommandPool.h>
#include <Vulkan/VulkanDescriptorPool.h>
#include <Vulkan/VulkanImage.h>
#include <Vulkan/VulkanInstance.h>
#include <Vulkan/VulkanMesh.h>
#include <Vulkan/VulkanPipeline.h>
#include <Vulkan/VulkanRenderPass.h>
#include <Vulkan/VulkanSampler.h>
#include <Vulkan/VulkanSkybox.h>
#include <Vulkan/VulkanSurface.h>
#include <Vulkan/VulkanSwapchain.h>
#include <vulkan/vulkan.hpp>

namespace Lucid::Vulkan
{

class VulkanRender : public Core::IRender
{
public:
    VulkanRender(const Core::IWindow& window, const Core::Scene& scene);
    ~VulkanRender() override;
    void DrawFrame() override;
    void AddNode(const Core::SceneNodePtr& node) override;
    bool ShouldClose() const override;

private:
    void RecreateSwapchain();
    void UpdateUniformBuffers();
    void RecordCommandBuffers();
    void SetupImgui();
    void DrawDockspace();
    void DrawOverlay();

    // Vulkan entities
    std::unique_ptr<VulkanInstance> mInstance;
    std::unique_ptr<VulkanDevice> mDevice;
    std::unique_ptr<VulkanSurface> mSurface;
    std::unique_ptr<VulkanSwapchain> mSwapchain;
    std::unique_ptr<VulkanRenderPass> mRenderPass;
    std::unique_ptr<VulkanPipeline> mMeshPipeline;
    std::unique_ptr<VulkanPipeline> mSkyboxPipeline;
    std::unique_ptr<VulkanCommandPool> mCommandPool;
    std::unique_ptr<VulkanDescriptorPool> mDescriptorPool;
    std::unique_ptr<VulkanImage> mResolveImage;
    std::unique_ptr<VulkanImage> mDepthImage;
    std::unique_ptr<VulkanSkybox> mSkybox;
    std::map<std::size_t, VulkanMesh> mMeshes;

    // Synchronization
    std::vector<vk::UniqueSemaphore> mImagePresentedSemaphores;
    std::vector<vk::UniqueSemaphore> mRenderFinishedSemaphores;
    std::vector<vk::UniqueFence> mInFlightFences;

    std::size_t mCurrentFrame = 0;
    const Core::IWindow* mWindow = nullptr;

    const Core::Scene& mScene;
    bool mShouldClose = false;

    // Settings
    bool mDrawSkybox = Defaults::DrawSkybox;
};

} // namespace Lucid::Vulkan
