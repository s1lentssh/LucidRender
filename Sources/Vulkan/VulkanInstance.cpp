#include "VulkanInstance.h"

#include <algorithm>

#include <Vulkan/VulkanDevice.h>
#include <Utils/Defaults.hpp>
#include <Utils/Logger.hpp>

namespace Lucid {

VulkanInstance::VulkanInstance(std::vector<const char*> requiredInstanceExtensions)
{
	auto unsupportedInstanceExtensions = VulkanInstance::GetUnsupportedExtensions(requiredInstanceExtensions);
	if (!unsupportedInstanceExtensions.empty())
	{
		Logger::List("Unsupported instance extensions", unsupportedInstanceExtensions);
		throw std::runtime_error("Unsupported instance extenstions");
	}

	if constexpr (Defaults::EnableValidationLayers)
	{
		auto unsupportedValidationLayers = VulkanInstance::GetUnsupportedLayers(mValidationLayers);
		if (!unsupportedValidationLayers.empty())
		{
			Logger::List("Unsupported validation layers", unsupportedInstanceExtensions);
			throw std::runtime_error("Unsupported validation layers");
		}
	}

	auto applicationInfo = vk::ApplicationInfo()
		.setPApplicationName(Defaults::ApplicationName.c_str())
		.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
		.setPEngineName(Defaults::EngineName.c_str())
		.setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
		.setApiVersion(VK_API_VERSION_1_2);

	if constexpr (Defaults::EnableValidationLayers)
	{
		requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	auto instanceCreateInfo = vk::InstanceCreateInfo()
		.setPApplicationInfo(&applicationInfo)
		.setEnabledExtensionCount(static_cast<std::uint32_t>(requiredInstanceExtensions.size()))
		.setPpEnabledExtensionNames(requiredInstanceExtensions.data());

	vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
	if constexpr (Defaults::EnableValidationLayers)
	{
		debugMessengerCreateInfo = VulkanInstance::ProvideDebugMessengerCreateInfo();

		instanceCreateInfo
			.setEnabledLayerCount(static_cast<std::uint32_t>(mValidationLayers.size()))
			.setPpEnabledLayerNames(mValidationLayers.data())
			.setPNext(&debugMessengerCreateInfo);
	}

	mInstance = vk::createInstanceUnique(instanceCreateInfo);
	Logger::Info("Vulkan instance created");

	if constexpr (Defaults::EnableValidationLayers)
	{
		RegisterDebugCallback();
	}
}

VulkanInstance::~VulkanInstance()
{
	if (mDebugMessenger.operator bool())
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)(mInstance->getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
		if (func != nullptr)
		{
			func(mInstance.get(), *(VkDebugUtilsMessengerEXT*)&mDebugMessenger, nullptr);
			Logger::Info("Debug messenger destroyed");
		}
	}
}

std::vector<VulkanDevice> VulkanInstance::GetDevices() const
{
	std::vector<vk::PhysicalDevice> devices = mInstance->enumeratePhysicalDevices();
	std::vector<VulkanDevice> wrappedDevices;
	std::transform(devices.begin(), devices.end(), std::back_inserter(wrappedDevices),
		[](const vk::PhysicalDevice& device) { return VulkanDevice(device); });
	return wrappedDevices;
}

VulkanDevice VulkanInstance::PickSuitableDeviceForSurface(const VulkanSurface& surface) const
{
	std::vector<VulkanDevice> availableDevices = this->GetDevices();

	auto availableDeviceIterator = std::find_if(availableDevices.begin(), availableDevices.end(), 
		[&surface](const VulkanDevice& device) { return device.IsSuitableForSurface(surface); });

	if (availableDeviceIterator == availableDevices.end())
	{
		throw std::runtime_error("Suitable device not found");
	}

	return std::move(*availableDeviceIterator);
}

std::vector<const char*> VulkanInstance::GetUnsupportedExtensions(std::vector<const char*> requiredInstanceExtensions) noexcept
{
	std::vector<vk::ExtensionProperties> availableExtensions = vk::enumerateInstanceExtensionProperties();

	// Remove required extension from list if it's available
	for (const auto& availableExtension : availableExtensions)
	{
		requiredInstanceExtensions.erase(
			std::remove_if(requiredInstanceExtensions.begin(), requiredInstanceExtensions.end(), [&availableExtension](const char* extension) {
				return std::string(availableExtension.extensionName.data()) == std::string(extension);
			}),
			requiredInstanceExtensions.end()
		);
	}

	return requiredInstanceExtensions;
}

std::vector<const char*> VulkanInstance::GetUnsupportedLayers(std::vector<const char*> requiredLayers) noexcept
{
	auto supportedLayers = vk::enumerateInstanceLayerProperties();

	// Remove required extension from list if it's available
	for (const auto& availableLayer : supportedLayers)
	{
		requiredLayers.erase(
			std::remove_if(requiredLayers.begin(), requiredLayers.end(), [&availableLayer](const char* layer) {
				return std::string(availableLayer.layerName.data()) == std::string(layer);
			}),
			requiredLayers.end()
		);
	}

	return requiredLayers;
}

void VulkanInstance::RegisterDebugCallback()
{
	auto createInfo = VulkanInstance::ProvideDebugMessengerCreateInfo();

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)mInstance->getProcAddr("vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		auto status = func(mInstance.get(), (VkDebugUtilsMessengerCreateInfoEXT*)&createInfo, nullptr, (VkDebugUtilsMessengerEXT*)&mDebugMessenger);
		Logger::Info("Debug messenger created");
	}
	else
	{
		throw std::runtime_error("Can't create vulkan debug messenger");
	}
}

vk::DebugUtilsMessengerCreateInfoEXT VulkanInstance::ProvideDebugMessengerCreateInfo() noexcept
{
	return vk::DebugUtilsMessengerCreateInfoEXT()
		.setMessageSeverity(
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
			/*vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | */
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
		.setMessageType(
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
		.setPfnUserCallback(DebugCallback);
}

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanInstance::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) noexcept
{
	messageSeverity;
	messageType;
	pUserData;

	Logger::Validation(pCallbackData->pMessage);
	return VK_FALSE;
}

}