#include "VulkanBuffer.h"

#include <Core/UniformBufferObject.h>
#include <Utils/Logger.hpp>
#include <Vulkan/VulkanCommandPool.h>
#include <Vulkan/VulkanDevice.h>

namespace Lucid::Vulkan
{

VulkanBuffer::VulkanBuffer(
    VulkanDevice& device,
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties,
    const std::string& name)
    : VulkanEntity(name, device.Handle())
{
    auto createInfo = vk::BufferCreateInfo().setSize(size).setUsage(usage).setSharingMode(vk::SharingMode::eExclusive);

    VulkanEntity::SetHandle(device.Handle().createBufferUnique(createInfo));
    vk::MemoryRequirements requirements = device.Handle().getBufferMemoryRequirements(Handle());
    std::uint32_t memoryType = FindMemoryType(device.GetPhysicalDevice(), requirements.memoryTypeBits, properties);

    auto allocateInfo = vk::MemoryAllocateInfo().setAllocationSize(requirements.size).setMemoryTypeIndex(memoryType);

    mMemory = device.Handle().allocateMemoryUnique(allocateInfo);

    mBufferSize = createInfo.size;
    device.Handle().bindBufferMemory(Handle(), mMemory.get(), 0);
}

void
VulkanBuffer::Write(const void* pixels, std::size_t size, std::size_t offset)
{
    if (size == 0)
    {
        size = mBufferSize;
    }

    void* deviceMemory = Device().mapMemory(mMemory.get(), 0, mBufferSize);
    std::memcpy(reinterpret_cast<std::byte*>(deviceMemory) + offset, pixels, size);
    Device().unmapMemory(mMemory.get());
}

std::uint32_t
VulkanBuffer::FindMemoryType(vk::PhysicalDevice& device, std::uint32_t filter, vk::MemoryPropertyFlags flags)
{
    vk::PhysicalDeviceMemoryProperties properties = device.getMemoryProperties();

    for (std::uint32_t i = 0; i < properties.memoryTypeCount; i++)
    {
        bool validType = filter & (1 << i);
        bool validProperties = (properties.memoryTypes.at(i).propertyFlags & flags) == flags;

        if (validType && validProperties)
        {
            return i;
        }
    }

    throw std::runtime_error("Can't find memory type");
}

VulkanVertexBuffer::VulkanVertexBuffer(
    VulkanDevice& device,
    VulkanCommandPool& manager,
    const std::vector<Core::Vertex>& vertices,
    const std::string& name)
    : VulkanBuffer(
        device,
        vertices.size() * sizeof(vertices.at(0)),
        vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        name)
    , mVerticesCount(vertices.size())
{
    VulkanBuffer stagingBuffer(
        device,
        vertices.size() * sizeof(vertices.at(0)),
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        "Staging buffer for " + name);

    // Copy vertex data to staging buffer (CPU -> CPU + GPU)
    stagingBuffer.Write(reinterpret_cast<const void*>(vertices.data()));

    // Copy staging buffer to vertex buffer (CPU + GPU -> GPU)
    Write(manager, stagingBuffer);
}

std::size_t
VulkanVertexBuffer::VerticesCount() const noexcept
{
    return mVerticesCount;
}

void
VulkanBuffer::Write(VulkanCommandPool& pool, const VulkanBuffer& buffer)
{
    pool.ExecuteSingleCommand(
        [&buffer, this](vk::CommandBuffer& commandBuffer)
        {
            auto copyRegion = vk::BufferCopy().setSize(mBufferSize);
            commandBuffer.copyBuffer(buffer.Handle(), Handle(), copyRegion);
        });
}

VulkanIndexBuffer::VulkanIndexBuffer(
    VulkanDevice& device,
    VulkanCommandPool& manager,
    const std::vector<std::uint32_t>& indices,
    const std::string& name)
    : VulkanBuffer(
        device,
        indices.size() * sizeof(indices.at(0)),
        vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        name)
    , mIndicesCount(indices.size())
{
    VulkanBuffer stagingBuffer(
        device,
        indices.size() * sizeof(indices.at(0)),
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        "Staging buffer for " + name);

    // Copy vertex data to staging buffer (CPU -> CPU + GPU)
    stagingBuffer.Write(reinterpret_cast<const void*>(indices.data()));

    // Copy staging buffer to vertex buffer (CPU + GPU -> GPU)
    Write(manager, stagingBuffer);
}

std::size_t
VulkanIndexBuffer::IndicesCount() const noexcept
{
    return mIndicesCount;
}

VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice& device, const std::string& name)
    : VulkanBuffer(
        device,
        sizeof(Core::UniformBufferObject),
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        name)
{
}

VulkanMaterialBuffer::VulkanMaterialBuffer(VulkanDevice& device, const std::string& name)
    : VulkanBuffer(
        device,
        sizeof(Core::MaterialBufferObject),
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        name)
{
}

} // namespace Lucid::Vulkan
