#include <Utils/Logger.hpp>
#include "Window.h"
#include <Vulkan/VulkanRender.h>

auto main() -> int try
{
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