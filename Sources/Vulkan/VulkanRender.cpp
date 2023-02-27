#include "VulkanRender.h"

#include <Core/UniformBufferObject.h>
#include <Utils/Defaults.hpp>
#include <Utils/Files.h>
#include <Utils/Logger.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>

namespace Lucid::Vulkan
{

VulkanRender::VulkanRender(const Core::IWindow& window, const Core::Scene& scene)
    : mWindow(&window)
    , mScene(scene)
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

    auto fenceCreateInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);

    for (std::uint32_t i = 0; i < Defaults::MaxFramesInFlight; i++)
    {
        mImagePresentedSemaphores.push_back(mDevice->Handle()->createSemaphoreUnique(semaphoreCreateInfo));
        mRenderFinishedSemaphores.push_back(mDevice->Handle()->createSemaphoreUnique(semaphoreCreateInfo));
        mInFlightFences.push_back(mDevice->Handle()->createFenceUnique(fenceCreateInfo));
    }

    mImagesInFlight.resize(Defaults::MaxFramesInFlight, {});

    // Skybox
    if constexpr (Defaults::DrawSkybox)
    {
        mSkybox = std::make_unique<VulkanSkybox>(*mDevice.get(), *mDescriptorPool.get(), *mCommandPool.get());
    }

    // ImGui
    SetupImgui();
}

void VulkanRender::SetupImgui()
{
    ImGui::CreateContext();

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding                     = ImVec2(8.00f, 8.00f);
    style.FramePadding                      = ImVec2(5.00f, 2.00f);
    style.CellPadding                       = ImVec2(6.00f, 6.00f);
    style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
    style.IndentSpacing                     = 25;
    style.ScrollbarSize                     = 15;
    style.GrabMinSize                       = 10;
    style.WindowBorderSize                  = 1;
    style.ChildBorderSize                   = 1;
    style.PopupBorderSize                   = 1;
    style.FrameBorderSize                   = 1;
    style.TabBorderSize                     = 1;
    style.WindowRounding                    = 0;
    style.ChildRounding                     = 0;
    style.FrameRounding                     = 0;
    style.PopupRounding                     = 0;
    style.ScrollbarRounding                 = 0;
    style.GrabRounding                      = 0;
    style.LogSliderDeadzone                 = 4;
    style.TabRounding                       = 0;

    ImGui_ImplGlfw_InitForVulkan(mWindow->Get(), true);
    ImGui_ImplVulkan_InitInfo info = {};
    info.Instance = mInstance->Handle().get();
    info.PhysicalDevice = mDevice->GetPhysicalDevice();
    info.Device = mDevice->Handle().get();
    info.QueueFamily = mDevice->FindGraphicsQueueFamily().value();
    info.Queue = mDevice->GetGraphicsQueue();
    info.DescriptorPool = mDescriptorPool->Handle().get();
    info.Subpass = 0;
    info.MinImageCount = 2;
    info.ImageCount = static_cast<std::uint32_t>(mSwapchain->GetImageCount());
    info.MSAASamples = static_cast<VkSampleCountFlagBits>(mDevice->GetMsaaSamples());

    ImGui_ImplVulkan_Init(&info, mRenderPass->Handle().get());
    mCommandPool->ExecuteSingleCommand([](vk::CommandBuffer& commandBuffer)
    {
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    });

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

VulkanRender::~VulkanRender()
{ 
    mDevice->Handle()->waitIdle();
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void
VulkanRender::DrawFrame()
{
    mDevice->Handle()->waitIdle();

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Top menu
    ImVec2 menuBarSize {};
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("About", nullptr)) {}
            if (ImGui::MenuItem("Close", "Esc")) mShouldClose = true;
            ImGui::EndMenu();
        }
        menuBarSize = ImGui::GetWindowSize();
        ImGui::EndMainMenuBar();
    }

    // Body
    ImGui::SetNextWindowSize({300.0f, static_cast<float>(mWindow->GetSize().y) - menuBarSize[1]});
    ImGui::SetNextWindowPos({static_cast<float>(mWindow->GetSize().x) - 300.0f, menuBarSize[1]});
    ImGui::Begin("Transform", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::End();
    // ImGui::ShowDemoWindow();

    ImGui::Render();

    mCommandPool->RecordCommandBuffers(
        *mSwapchain.get(),
        *mRenderPass.get(),
        [this](vk::CommandBuffer& commandBuffer)
        {
            // Skybox
            if constexpr (Defaults::DrawSkybox)
            {
                commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mSkyboxPipeline->Handle().get());
                mSkybox->Draw(commandBuffer, *mSkyboxPipeline.get());
            }

            // Push constants
            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mMeshPipeline->Handle().get());

            static Core::PushConstants constants;
            constants.ambientColor = glm::make_vec3(Defaults::AmbientColor.data());
            constants.ambientFactor = 1000.0f;
            constants.lightPosition = glm::vec3(400.0, 50.0, 400.0);
            constants.lightColor = glm::vec3(4.5, 4.5, 5.0);
            commandBuffer.pushConstants(
                mMeshPipeline->Layout(),
                vk::ShaderStageFlagBits::eFragment,
                0,
                sizeof(Core::PushConstants),
                &constants);

            // Geometry
            for (const VulkanMesh& mesh : mMeshes)
            {
                mesh.Draw(commandBuffer, *mMeshPipeline.get());
            }

            // ImGui
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
        });

    // Render frame
    auto result = mDevice->Handle()->waitForFences(
        mInFlightFences[mCurrentFrame].get(), true, std::numeric_limits<std::uint64_t>::max());
    (void)result;

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
        auto result2 = mDevice->Handle()->waitForFences(
            mImagesInFlight[imageIndex], true, std::numeric_limits<std::uint64_t>::max());
        (void)result2;
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

void
VulkanRender::AddAsset(const Core::Asset& asset)
{
    mMeshes.push_back(
        VulkanMesh(*mDevice.get(), *mDescriptorPool.get(), *mCommandPool.get(), asset.GetTexture(), asset.GetMesh()));
    RecordCommandBuffers();
}

bool 
VulkanRender::ShouldClose() const
{
    return mShouldClose;
}

void
VulkanRender::RecreateSwapchain()
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

    // Create pipelines
    mMeshPipeline
        = VulkanPipeline::Default(*mDevice.get(), mSwapchain->GetExtent(), *mRenderPass.get(), *mDescriptorPool.get());

    if constexpr (Defaults::DrawSkybox)
    {
        mSkyboxPipeline = VulkanPipeline::Skybox(*mDevice.get(), mSwapchain->GetExtent(), *mRenderPass.get(), *mDescriptorPool.get());
    }

    // Create depth image
    mDepthImage = VulkanImage::CreateDepthImage(
        *mDevice.get(), mSwapchain->GetExtent(), mDevice->FindSupportedDepthFormat(), vk::ImageAspectFlagBits::eDepth);

    // Create MSAA RenderTarget
    mResolveImage = VulkanImage::CreateImage(*mDevice.get(), mSwapchain->GetImageFormat(), mSwapchain->GetExtent());

    // Create framebuffers for swapchain
    mSwapchain->CreateFramebuffers(*mRenderPass.get(), *mDepthImage.get(), *mResolveImage.get());

    RecordCommandBuffers();
}

void
VulkanRender::UpdateUniformBuffers()
{
    vk::Extent2D extent = mSwapchain->GetExtent();
    float aspectRatio = static_cast<float>(extent.width) / static_cast<float>(extent.height);

    Core::UniformBufferObject ubo;
    ubo.view = mScene.GetCamera()->Transform();
    ubo.projection
        = glm::perspective(glm::radians(mScene.GetCamera()->FieldOfView()), aspectRatio, 0.01f, 1'000'000.0f);
    ubo.projection[1][1] *= -1;

    for (std::size_t i = 0; i < mMeshes.size(); i++)
    {
        ubo.model = mScene.GetAssets().at(i).Transform();
        mMeshes.at(i).UpdateTransform(ubo);
    }

    if constexpr (Defaults::DrawSkybox)
    {
        mSkybox->UpdateTransform(ubo);
    }
}

void
VulkanRender::RecordCommandBuffers()
{
    mCommandPool->RecreateCommandBuffers(*mSwapchain.get());
}

} // namespace Lucid::Vulkan
