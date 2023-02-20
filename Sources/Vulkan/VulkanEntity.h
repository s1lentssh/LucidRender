#pragma once

#include <iostream>
#include <type_traits>

#include <vulkan/vulkan.hpp>
// #include <concepts>

namespace Lucid::Vulkan
{

/*
template<typename T>
concept HasGetMethod = requires { T::get; };
*/

template <typename HandleType> class VulkanEntity
{
public:
    [[nodiscard]] const HandleType& Handle() const { return mHandle; }

protected:
    HandleType mHandle;
};

} // namespace Lucid::Vulkan
