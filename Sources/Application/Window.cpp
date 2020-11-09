#include "Window.h"

#include <Utils/Defaults.hpp>

Window::Window()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    mWindow = glfwCreateWindow(Defaults::Width, Defaults::Height, Defaults::ApplicationName.c_str(), nullptr, nullptr);
}

Window::~Window()
{
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

bool Window::ShouldClose() const noexcept
{
    return glfwWindowShouldClose(mWindow);
}

void Window::PollEvents() noexcept
{
    glfwPollEvents();
}

std::vector<const char*> Window::GetRequiredInstanceExtensions() const noexcept
{
    uint32_t glfwExtensionsCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

    std::vector<const char*> result(glfwExtensions, glfwExtensions + glfwExtensionsCount);

    return result;
}

void* Window::GetHandle() const noexcept
{
    return glfwGetWin32Window(mWindow);
}

lucid::Vector2d<std::uint32_t> Window::GetSize() const noexcept
{
    int width, height;
    glfwGetWindowSize(mWindow, &width, &height);
    return { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) };
}
