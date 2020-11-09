#pragma once

namespace lucid {

template<typename T>
struct Vector2d 
{
    T x;
    T y;
};

class IWindow
{
public:
    [[nodiscard]] virtual void* GetHandle() const noexcept = 0;
    [[nodiscard]] virtual std::vector<const char*> GetRequiredInstanceExtensions() const noexcept = 0;
    [[nodiscard]] virtual Vector2d<std::uint32_t> GetSize() const noexcept = 0;
};

}