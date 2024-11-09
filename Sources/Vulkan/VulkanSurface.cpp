#include "VulkanSurface.h"

#include <Core/Interfaces.h>
#include <Utils/Logger.hpp>
#include <Vulkan/VulkanInstance.h>

#ifdef __linux__
#include <X11/Xlib-xcb.h>
#endif

namespace Lucid::Vulkan
{

VulkanSurface::VulkanSurface(VulkanInstance& instance, const Core::IWindow& window, const std::string& name)
    : VulkanEntity(name, 1)
{

#ifdef _WIN32
    auto createInfo = vk::Win32SurfaceCreateInfoKHR()
                          .setHwnd(static_cast<HWND>(window.Handle()))
                          .setHinstance(GetModuleHandle(nullptr));

    VulkanEntity::SetHandle(instance.Handle().createWin32SurfaceKHRUnique(createInfo));
#endif

#ifdef __linux__
    auto createInfo = vk::XcbSurfaceCreateInfoKHR()
                          .setWindow(static_cast<std::uint32_t>(window.Handle()))
                          .setConnection(XGetXCBConnection(reinterpret_cast<Display*>(window.Display())));

    VulkanEntity::SetHandle(instance.Handle().createXcbSurfaceKHRUnique(createInfo));
#endif

#ifdef __APPLE__
    auto createInfo = vk::MetalSurfaceCreateInfoEXT().setPLayer(window.Handle());

    VulkanEntity::SetHandle(instance.Handle().createMetalSurfaceEXTUnique(createInfo));
#endif

    if (Handle().operator bool())
    {
        LoggerInfo << "Surface created";
    }
    else
    {
        LoggerError << "Surface not created";
    }
}

} // namespace Lucid::Vulkan
