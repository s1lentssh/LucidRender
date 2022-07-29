#include "VulkanSurface.h"

#include <Vulkan/VulkanInstance.h>
#include <Utils/Logger.hpp>
#include <Core/Interfaces.h>

#ifdef __linux__
#include <X11/Xlib-xcb.h>
#endif

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
	auto createInfo = vk::XcbSurfaceCreateInfoKHR()
		.setWindow(window.Handle())
		.setConnection(XGetXCBConnection(reinterpret_cast<Display*>(window.Display())));

	mHandle = instance.Handle()->createXcbSurfaceKHRUnique(createInfo);
#endif

#ifdef __APPLE__
	auto createInfo = vk::MetalSurfaceCreateInfoEXT()
		.setPLayer(window.Handle());

	mHandle = instance.Handle()->createMetalSurfaceEXTUnique(createInfo);
#endif

	if (mHandle->operator bool()) 
	{
		LoggerInfo << "Surface created";
	} 
	else 
	{
		LoggerError << "Surface not created";
	}
}

}
