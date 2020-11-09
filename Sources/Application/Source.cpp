#include <Utils/Logger.hpp>
#include "Window.h"
#include <Vulkan/VulkanRender.h>

auto main() -> int 
{
    Window window;

    std::unique_ptr<lucid::VulkanRender> render;
    try
    {
        render = std::make_unique<lucid::VulkanRender>(window);
    }
    catch (const std::exception & ex)
    {
        Logger::Error(ex.what());
        return EXIT_FAILURE;
    }

    while (!window.ShouldClose())
    {
        window.PollEvents();
    }

    return EXIT_SUCCESS;
}