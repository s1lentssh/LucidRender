#include "VulkanSurface.h"

#include <Vulkan/VulkanInstance.h>
#include <Utils/Logger.hpp>
#include <Core/Interfaces.h>

namespace Lucid::Vulkan
{

VulkanSurface::VulkanSurface(VulkanInstance& instance, const Core::IWindow& window)
{

#ifdef _WIN32
	auto createInfo = vk::Win32SurfaceCreateInfoKHR()
		.setHwnd(static_cast<HWND>(window.Handle()))
		.setHinstance(GetModuleHandle(nullptr));

	mHandle = instance.Handle()->createWin32SurfaceKHRUnique(createInfo);
#endif

#ifdef __linux__
	auto createInfo = vk::XlibSurfaceCreateInfoKHR()
		.setWindow((Window)(window.Handle()));

	mHandle = instance.Handle()->createXlibSurfaceKHRUnique(createInfo);
#endif

	Logger::Info("Surface created");
}

}
