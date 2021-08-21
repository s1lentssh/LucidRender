#pragma once

#include <filesystem>
#include <Core/Types.h>

namespace Lucid::Core 
{

class IWindow
{
public:
    #ifdef _WIN32
    [[nodiscard]] virtual void* Handle() const noexcept = 0;
    #endif

    #ifdef __linux__
    [[nodiscard]] virtual unsigned int Handle() const noexcept = 0;
    [[nodiscard]] virtual void* Display() const noexcept = 0;
    #endif

    [[nodiscard]] virtual std::vector<const char*> GetRequiredInstanceExtensions() const noexcept = 0;
    [[nodiscard]] virtual Vector2d<std::uint32_t> GetSize() const noexcept = 0;
    [[nodiscard]] virtual bool ShouldClose() const noexcept = 0;
    virtual void SetIcon(const std::filesystem::path& path) = 0;
    virtual void PollEvents() const noexcept = 0;
    virtual void WaitEvents() const noexcept = 0;
    virtual ~IWindow() = default;
};

class IRender
{
public:
    virtual void DrawFrame() = 0;
    virtual void AddAsset(const Core::Asset& asset) = 0;
    virtual ~IRender() = default;
};

}