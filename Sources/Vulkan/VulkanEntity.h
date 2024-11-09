#pragma once

#include <concepts>
#include <iostream>
#include <type_traits>

#include <vulkan/vulkan.hpp>

namespace Lucid::Vulkan
{

template <typename T, typename = void> struct HasGetMethod : std::false_type
{
};

template <typename T> struct HasGetMethod<T, std::void_t<decltype(std::declval<T>().get())>> : std::true_type
{
};

template <typename HandleType> class VulkanEntity
{
public:
    [[nodiscard]] auto& Handle()
    {
        if constexpr (HasGetMethod<HandleType>::value)
        {
            return mHandle.get();
        }
        else
        {
            return mHandle;
        }
    }
    [[nodiscard]] auto& Handle() const
    {
        if constexpr (HasGetMethod<HandleType>::value)
        {
            return mHandle.get();
        }
        else
        {
            return mHandle;
        }
    }

    void SetHandle(HandleType&& handle)
    {
        mHandle = std::move(handle);

        if (mDevice)
        {
            auto func = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(
                mDevice.getProcAddr("vkSetDebugUtilsObjectNameEXT"));
            if (func != nullptr)
            {
                vk::DebugUtilsObjectNameInfoEXT nameInfo = {};

                if constexpr (HasGetMethod<HandleType>::value)
                {
                    auto handleType = mHandle.get();
                    using CType = typename decltype(handleType)::CType;
                    nameInfo.objectType = handleType.objectType;
                    nameInfo.objectHandle = reinterpret_cast<std::uint64_t>(static_cast<CType>(handleType));
                }
                else
                {
                    using CType = typename decltype(mHandle)::CType;
                    nameInfo.objectType = mHandle.objectType;
                    nameInfo.objectHandle = reinterpret_cast<std::uint64_t>(static_cast<CType>(mHandle));
                }

                nameInfo.pObjectName = mDebugName.c_str();

                func(mDevice, &nameInfo.operator VkDebugUtilsObjectNameInfoEXT&());
            }
        }
    }

    void SetDevice(const vk::Device& device) { mDevice = device; }

    VulkanEntity(const std::string& name, const vk::Device& device)
        : mDevice(device)
        , mDebugName(name)
    {
    }

    VulkanEntity(const std::string& name, std::size_t marker)
        : mDebugName(name)
    {
        (void)marker;
    }

    vk::Device Device()
    {
        if (!mDevice.operator bool())
        {
            throw std::runtime_error("Tried to get NULL device");
        }
        return mDevice;
    }

private:
    vk::Device mDevice = {};
    HandleType mHandle;
    std::string mDebugName;
};

} // namespace Lucid::Vulkan
