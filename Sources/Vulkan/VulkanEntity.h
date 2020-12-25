#pragma once

namespace Lucid::Vulkan
{

template<typename HandleType>
class VulkanEntity
{
public:
	[[nodiscard]] const auto& Handle() const { return mHandle.get(); }

protected:
	HandleType mHandle;
};

}