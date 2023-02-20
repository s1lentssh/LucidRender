#include "Window.h"

#include <Utils/Defaults.hpp>
#include <Core/InputController.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <map>

#ifdef __APPLE__
#include <Utils/MacOS/Interface.h>
#endif

namespace Lucid
{

Window::Window()
{
    glfwInit();

    // If no api is selected, windows couldn't be painted black on load
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mWindow = glfwCreateWindow(Defaults::Width, Defaults::Height, Defaults::ApplicationName.c_str(), nullptr, nullptr);

    if (mWindow == nullptr)
    {
        throw std::runtime_error("Can't create window");
    }

    // Paint it black on load
	glfwSwapBuffers(mWindow);

    glfwSetKeyCallback(mWindow, OnKeyPressed);
    glfwSetCursorPosCallback(mWindow, OnCursorMoved);
    glfwSetScrollCallback(mWindow, OnScrolled);
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
    else
    {
        std::map<int, Core::InputController::Key> keymap = {
            {GLFW_KEY_W, Core::InputController::Key::Up},
            {GLFW_KEY_S, Core::InputController::Key::Down},
            {GLFW_KEY_A, Core::InputController::Key::Left},
            {GLFW_KEY_D, Core::InputController::Key::Right}
        };

        if (keymap.contains(key))
        {
            if (action == GLFW_PRESS)
            {
                Core::InputController::Instance().KeyPressed(keymap.at(key));
            }
            else if (action == GLFW_RELEASE)
            {
                Core::InputController::Instance().KeyReleased(keymap.at(key));
            }
        }
    }
}

void Window::OnCursorMoved(GLFWwindow* window, double x, double y)
{
    (void)window;
    Core::InputController::Instance().MouseMoved(static_cast<float>(x), static_cast<float>(y));
}

void Window::OnScrolled(GLFWwindow* window, double x, double y)
{
    (void)window;
    Core::InputController::Instance().MouseScrolled(static_cast<float>(x), static_cast<float>(y));
}

std::vector<const char*> Window::GetRequiredInstanceExtensions() const noexcept
{
    uint32_t glfwExtensionsCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
    return { glfwExtensions, glfwExtensions + glfwExtensionsCount };
}

#ifdef _WIN32
void* Window::Handle() const noexcept
{
    return glfwGetWin32Window(mWindow);
}
#endif

#ifdef __linux__
unsigned int Window::Handle() const noexcept
{
    return glfwGetX11Window(mWindow);
}

void* Window::Display() const noexcept
{
    return glfwGetX11Display();
}
#endif

#ifdef __APPLE__
void* Window::Handle() const noexcept
{
    void* windowHandle = glfwGetCocoaWindow(mWindow);
    void* viewHandle = getMetalLayer(windowHandle);
    return viewHandle;
}
#endif

Core::Vector2d<std::uint32_t> Window::GetSize() const noexcept
{
    int width, height;
    glfwGetWindowSize(mWindow, &width, &height);
    return { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) };
}

}