#include "Window.h"

#include <Utils/Defaults.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Lucid
{

Window::Window()
{
    glfwInit();

    // If no api is selected, windows couldn't be painted black on load
    // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mWindow = glfwCreateWindow(Defaults::Width, Defaults::Height, Defaults::ApplicationName.c_str(), nullptr, nullptr);

    if (mWindow == nullptr)
    {
        throw std::runtime_error("Can't create window");
    }

    // Paint it black on load
	glfwSwapBuffers(mWindow);

    glfwSetKeyCallback(mWindow, OnKeyPressed);
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

void Window::PollEvents() const noexcept
{
    glfwPollEvents();
}

void Window::WaitEvents() const noexcept
{
    glfwWaitEvents();
}

void Window::SetIcon(const std::filesystem::path& path)
{
    GLFWimage icon;
    icon.pixels = stbi_load(path.string().c_str(), &icon.width, &icon.height, nullptr, 4);
    glfwSetWindowIcon(mWindow, 1, &icon);
    stbi_image_free(icon.pixels);
}

void Window::OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)mods;
    (void)scancode;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

std::vector<const char*> Window::GetRequiredInstanceExtensions() const noexcept
{
    uint32_t glfwExtensionsCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
    return { glfwExtensions, glfwExtensions + glfwExtensionsCount };
}

void* Window::Handle() const noexcept
{
#ifdef _WIN32
    return glfwGetWin32Window(mWindow);
#endif

#ifdef __linux__
    return reinterpret_cast<void*>(glfwGetX11Window(mWindow));
#endif
}

Vector2d<std::uint32_t> Window::GetSize() const noexcept
{
    int width, height;
    glfwGetWindowSize(mWindow, &width, &height);
    return { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) };
}

}