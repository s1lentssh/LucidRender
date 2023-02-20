#pragma once

#include <type_traits>
#include <vulkan/vulkan.hpp>
#include <iostream>
// #include <concepts>

namespace Lucid::Vulkan
{

/*
template<typename T>
concept HasGetMethod = requires { T::get; };
*/

template<typename HandleType>
class VulkanEntity
{
public:
	[[nodiscard]] const HandleType& Handle() const {
		return mHandle;
	}

protected:
	HandleType mHandle;
};

}
