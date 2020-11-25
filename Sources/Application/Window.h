#pragma once

#include <memory>
#include <vector>
#include <Utils/Interfaces.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Lucid 
{
class Window : public Lucid::IWindow
{
public:
    Window();
    ~Window();
    
    [[nodiscard]] bool ShouldClose() const noexcept override;
    [[nodiscard]] std::vector<const char*> GetRequiredInstanceExtensions() const noexcept override;
    [[nodiscard]] void* Handle() const noexcept override;
    [[nodiscard]] Vector2d<std::uint32_t> GetSize() const noexcept override;
    void PollEvents() const noexcept override;
    void WaitEvents() const noexcept override;
    void SetIcon(const std::filesystem::path& path) override;

private:
    GLFWwindow* mWindow = nullptr;
};

}