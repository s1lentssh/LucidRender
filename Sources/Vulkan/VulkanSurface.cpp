#include "VulkanSurface.h"

#include <Vulkan/VulkanInstance.h>
#include <Utils/Interfaces.hpp>
#include <Utils/Logger.hpp>

namespace Lucid
{

VulkanSurface::VulkanSurface(VulkanInstance& instance, const IWindow& window)
{
	auto createInfo = vk::Win32SurfaceCreateInfoKHR()
		.setHwnd(static_cast<HWND>(window.Handle()))
		.setHinstance(GetModuleHandle(nullptr));

	mSurface = instance.Handle()->createWin32SurfaceKHRUnique(createInfo);
	Logger::Info("Surface created");
}

}
