#pragma once

#include <Vulkan/VulkanEntity.h>
#include <vulkan/vulkan.hpp>

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanSurface;

class VulkanInstance : public VulkanEntity<vk::UniqueInstance>
{
public:
    VulkanInstance(std::vector<const char*> requiredInstanceExtensions);
    ~VulkanInstance();

    [[nodiscard]] VulkanDevice PickSuitableDeviceForSurface(const VulkanSurface& surface) const;

private:
    [[nodiscard]] static std::vector<const char*>
    GetUnsupportedExtensions(std::vector<const char*> requiredExtensions) noexcept;
    [[nodiscard]] static std::vector<const char*>
    GetUnsupportedLayers(std::vector<const char*> requiredLayers) noexcept;
    [[nodiscard]] std::vector<VulkanDevice> GetDevices() const;

    // Validation
    void RegisterDebugCallback();
    [[nodiscard]] static vk::DebugUtilsMessengerCreateInfoEXT ProvideDebugMessengerCreateInfo() noexcept;

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) noexcept;

    const std::vector<const char*> mValidationLayers = { "VK_LAYER_KHRONOS_validation" };
    vk::DebugUtilsMessengerEXT mDebugMessenger;
};

} // namespace Lucid::Vulkan