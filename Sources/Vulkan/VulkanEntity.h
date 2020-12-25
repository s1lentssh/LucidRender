#pragma once

#include <type_traits>
#include <vulkan/vulkan.hpp>

namespace Lucid::Vulkan
{

template<typename>
constexpr std::false_type HasGetHelper(long);

template<typename T>
constexpr auto HasGetHelper(int) -> decltype(std::declval<T>().get(), std::true_type{} );

template<typename T>
using HasGet = decltype(HasGetHelper<T>(0));

template<typename HandleType>
class VulkanEntity
{
public:
	VulkanEntity() = default;

	[[nodiscard]] const auto& Handle() const { 
		if constexpr (HasGet<HandleType>{})
		{
			return mHandle.get();
		}
		else
		{
			return mHandle;
		}
	}

protected:
	HandleType mHandle;
};

template class VulkanEntity<vk::Image>;
template class VulkanEntity<vk::UniqueImage>;

}