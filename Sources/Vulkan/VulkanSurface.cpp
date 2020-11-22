#include "VulkanSurface.h"

#include <Vulkan/VulkanInstance.h>
#include <Utils/Interfaces.hpp>
#include <Utils/Logger.hpp>

namespace Lucid
{

VulkanSurface::VulkanSurface(VulkanInstance& instance, const IWindow& window)
{

#ifdef _WIN32
	auto createInfo = vk::Win32SurfaceCreateInfoKHR()
		.setHwnd(static_cast<HWND>(window.Handle()))
		.setHinstance(GetModuleHandle(nullptr));

	mSurface = instance.Handle()->createWin32SurfaceKHRUnique(createInfo);
#endif

#ifdef __linux__
	auto createInfo = vk::XlibSurfaceCreateInfoKHR()
		.setWindow((Window)(window.Handle()));

	mSurface = instance.Handle()->createXlibSurfaceKHRUnique(createInfo);
#endif

	Logger::Info("Surface created");
}

}
