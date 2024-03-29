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
    ~Window() override;

    [[nodiscard]] bool ShouldClose() const noexcept override;
    [[nodiscard]] std::vector<const char*> GetRequiredInstanceExtensions() const noexcept override;

#ifdef _WIN32
    [[nodiscard]] void* Handle() const noexcept override;
#endif

#ifdef __linux__
    [[nodiscard]] std::uint64_t Handle() const noexcept override;
    [[nodiscard]] void* Display() const noexcept override;
#endif

#ifdef __APPLE__
    [[nodiscard]] void* Handle() const noexcept override;
#endif

    GLFWwindow* Get() const noexcept override { return mWindow; }

    [[nodiscard]] Core::Vector2d<std::uint32_t> GetSize() const noexcept override;
    void PollEvents() const noexcept override;
    void WaitEvents() const noexcept override;
    void SetIcon(const std::filesystem::path& path) override;

    static void OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void OnCursorMoved(GLFWwindow* window, double x, double y);
    static void OnScrolled(GLFWwindow* window, double x, double y);
    static void OnMouseButton(GLFWwindow* window, int button, int action, int mods);

private:
    GLFWwindow* mWindow = nullptr;
};

} // namespace Lucid
