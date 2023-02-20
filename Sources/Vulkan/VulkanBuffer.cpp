#include "VulkanBuffer.h"

#include <Core/UniformBufferObject.h>
#include <Vulkan/VulkanCommandPool.h>
#include <Vulkan/VulkanDevice.h>

namespace Lucid::Vulkan
{

VulkanBuffer::VulkanBuffer(
    VulkanDevice& device,
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties)
    : mDevice(device)
{
    auto createInfo = vk::BufferCreateInfo().setSize(size).setUsage(usage).setSharingMode(vk::SharingMode::eExclusive);

    mHandle = device.Handle()->createBufferUnique(createInfo);
    vk::MemoryRequirements requirements = device.Handle()->getBufferMemoryRequirements(Handle().get());
    std::uint32_t memoryType = FindMemoryType(mDevice, requirements.memoryTypeBits, properties);

    auto allocateInfo = vk::MemoryAllocateInfo().setAllocationSize(requirements.size).setMemoryTypeIndex(memoryType);

    mMemory = device.Handle()->allocateMemoryUnique(allocateInfo);
    mBufferSize = createInfo.size;
    device.Handle()->bindBufferMemory(Handle().get(), mMemory.get(), 0);
}

void
VulkanBuffer::Write(const void* pixels)
{
    void* deviceMemory = mDevice.Handle()->mapMemory(mMemory.get(), 0, mBufferSize);
    std::memcpy(deviceMemory, pixels, mBufferSize);
    mDevice.Handle()->unmapMemory(mMemory.get());
}

std::uint32_t
VulkanBuffer::FindMemoryType(VulkanDevice& device, std::uint32_t filter, vk::MemoryPropertyFlags flags)
{
    vk::PhysicalDeviceMemoryProperties properties = device.GetPhysicalDevice().getMemoryProperties();

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
    const std::vector<Core::Vertex>& vertices)
    : VulkanBuffer(
        device,
        vertices.size() * sizeof(vertices.at(0)),
        vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        vk::MemoryPropertyFlagBits::eDeviceLocal)
    , mVerticesCount(vertices.size())
{
    VulkanBuffer stagingBuffer(
        device,
        vertices.size() * sizeof(vertices.at(0)),
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

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
            commandBuffer.copyBuffer(buffer.Handle().get(), Handle().get(), copyRegion);
        });
}

VulkanIndexBuffer::VulkanIndexBuffer(
    VulkanDevice& device,
    VulkanCommandPool& manager,
    const std::vector<std::uint32_t>& indices)
    : VulkanBuffer(
        device,
        indices.size() * sizeof(indices.at(0)),
        vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        vk::MemoryPropertyFlagBits::eDeviceLocal)
    , mIndicesCount(indices.size())
{
    VulkanBuffer stagingBuffer(
        device,
        indices.size() * sizeof(indices.at(0)),
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

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

VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice& device)
    : VulkanBuffer(
        device,
        sizeof(Core::UniformBufferObject),
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
{
}

} // namespace Lucid::Vulkan
