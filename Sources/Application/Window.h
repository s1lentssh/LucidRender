#pragma once

#include <memory>
#include <vector>
#include <Utils/Interfaces.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

class Window : public Lucid::IWindow
{
public:
    Window();
    ~Window();
    
    [[nodiscard]] bool ShouldClose() const noexcept override;
    void PollEvents() noexcept override;
    [[nodiscard]] std::vector<const char*> GetRequiredInstanceExtensions() const noexcept override;
    [[nodiscard]] void* GetHandle() const noexcept override;
    [[nodiscard]] Lucid::Vector2d<std::uint32_t> GetSize() const noexcept override;

private:
    GLFWwindow* mWindow = nullptr;
};