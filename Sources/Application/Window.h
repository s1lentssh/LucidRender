#pragma once

#include <memory>
#include <vector>
#include <Core/Interfaces.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Lucid 
{

class Window : public Core::IWindow
{
public:
    Window();
    ~Window();
    
    [[nodiscard]] bool ShouldClose() const noexcept override;
    [[nodiscard]] std::vector<const char*> GetRequiredInstanceExtensions() const noexcept override;
    [[nodiscard]] void* Handle() const noexcept override;
    [[nodiscard]] Core::Vector2d<std::uint32_t> GetSize() const noexcept override;
    void PollEvents() const noexcept override;
    void WaitEvents() const noexcept override;
    void SetIcon(const std::filesystem::path& path) override;

    static void OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    GLFWwindow* mWindow = nullptr;
};

}