#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanDevice.h"

namespace lucid {

class VulkanInstance
{
public:
	VulkanInstance(std::vector<const char*> requiredInstanceExtensions);
	~VulkanInstance();

	[[nodiscard]] VulkanDevice PickSuitableDeviceForSurface(const VulkanSurface& surface) const;

	[[nodiscard]] vk::UniqueInstance& Handle() { return mInstance; };

private:
	[[nodiscard]] static std::vector<const char*> GetUnsupportedExtensions(std::vector<const char*> requiredExtensions) noexcept;
	[[nodiscard]] static std::vector<const char*> GetUnsupportedLayers(std::vector<const char*> requiredLayers) noexcept;
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
	
	// Members
	vk::UniqueInstance mInstance;
};

}