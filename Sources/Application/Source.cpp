#include "Window.h"
#include <Utils/Logger.hpp>
#include <Utils/Defaults.hpp>
#include <Vulkan/VulkanRender.h>

auto main() -> int try
{
    if constexpr (Defaults::CurrentPlatform == Defaults::Platform::Windows)
    {
        SetConsoleTitle((Defaults::ApplicationName + " Console").c_str());
    }

    std::unique_ptr<Lucid::IWindow> window = std::make_unique<Window>();
    window->SetIcon("Resources/Icons/AppIcon.png");

    std::unique_ptr<Lucid::IRender> render = std::make_unique<Lucid::VulkanRender>(*window.get());

    while (!window->ShouldClose())
    {
        window->PollEvents();
        render->DrawFrame();
    }

    return EXIT_SUCCESS;
}
catch (const std::exception & ex)
{
    Logger::Error(ex.what());
    return EXIT_FAILURE;
}