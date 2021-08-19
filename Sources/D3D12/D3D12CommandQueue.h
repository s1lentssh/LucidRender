#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <functional>
#include <queue>
#include <D3D12/D3D12Entity.h>
#include <D3D12/D3D12Device.h>
#include <D3D12/D3D12Fence.h>
#include <D3D12/D3D12CommandAllocator.h>

namespace Lucid::D3D12
{

using namespace Microsoft::WRL;

class D3D12CommandQueue : public D3D12Entity<ComPtr<ID3D12CommandQueue>>
{
public:
	D3D12CommandQueue(D3D12Device& device, D3D12_COMMAND_LIST_TYPE type);
	ComPtr<ID3D12GraphicsCommandList2> GetCommandList();
	std::uint64_t ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList);
	D3D12Fence& GetFence() { return mFence; }

private:
	ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
	ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(ComPtr<ID3D12CommandAllocator>& allocator);

private:
	D3D12Device& mDevice;
	D3D12_COMMAND_LIST_TYPE mType;
	D3D12Fence mFence;

	struct CommandAllocatorEntry
	{
		std::uint64_t fenceValue;
		ComPtr<ID3D12CommandAllocator> allocator;
	};

	std::queue<CommandAllocatorEntry> mCommandAllocatorQueue;
	std::queue<ComPtr<ID3D12GraphicsCommandList2>> mCommandListQueue;
};

}