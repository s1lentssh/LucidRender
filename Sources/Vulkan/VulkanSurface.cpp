#include "VulkanSurface.h"

#include <Utils/Interfaces.hpp>
#include <Utils/Logger.hpp>

namespace lucid
{

VulkanSurface::VulkanSurface(VulkanInstance& instance, void* windowHandle)
{
	auto createInfo = vk::Win32SurfaceCreateInfoKHR()
		.setHwnd(static_cast<HWND>(windowHandle))
		.setHinstance(GetModuleHandle(nullptr));

	mSurface = instance.Handle()->createWin32SurfaceKHRUnique(createInfo);
	Logger::Info("Surface created");
}

}
