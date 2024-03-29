#pragma once

#include <Core/Vertex.h>
#include <Vulkan/VulkanEntity.h>
#include <vulkan/vulkan.hpp>

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanCommandPool;
struct VulkanVertex;

class VulkanBuffer : public VulkanEntity<vk::UniqueBuffer>
{
public:
    VulkanBuffer(
        VulkanDevice& device,
        vk::DeviceSize size,
        vk::BufferUsageFlags usage,
        vk::MemoryPropertyFlags properties);
    void Write(const void* pixels, std::size_t size = 0, std::size_t offset = 0);

    [[nodiscard]] static std::uint32_t
    FindMemoryType(VulkanDevice& device, std::uint32_t filter, vk::MemoryPropertyFlags flags);

protected:
    void Write(VulkanCommandPool& manager, const VulkanBuffer& buffer);

    vk::UniqueDeviceMemory mMemory;
    std::size_t mBufferSize;
    VulkanDevice& mDevice;
};

class VulkanVertexBuffer : public VulkanBuffer
{
public:
    VulkanVertexBuffer(VulkanDevice& device, VulkanCommandPool& manager, const std::vector<Core::Vertex>& vertices);
    [[nodiscard]] std::size_t VerticesCount() const noexcept;

private:
    std::size_t mVerticesCount = 0;
};

class VulkanIndexBuffer : public VulkanBuffer
{
public:
    VulkanIndexBuffer(VulkanDevice& device, VulkanCommandPool& manager, const std::vector<std::uint32_t>& indices);
    [[nodiscard]] std::size_t IndicesCount() const noexcept;

private:
    std::size_t mIndicesCount = 0;
};

class VulkanUniformBuffer : public VulkanBuffer
{
public:
    VulkanUniformBuffer(VulkanDevice& device);
};

} // namespace Lucid::Vulkan
