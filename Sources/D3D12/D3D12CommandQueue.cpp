#include <D3D12/D3D12CommandQueue.h>
#include <D3D12/D3D12Utils.h>
#include <optional>

namespace Lucid::D3D12
{

D3D12CommandQueue::D3D12CommandQueue(D3D12Device& device, D3D12_COMMAND_LIST_TYPE type)
	: mDevice(device)
	, mType(type)
	, mFence(device)
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	device.Handle()->CreateCommandQueue(&desc, IID_PPV_ARGS(&mHandle));
}

ComPtr<ID3D12GraphicsCommandList2> D3D12CommandQueue::GetCommandList()
{
	ComPtr<ID3D12CommandAllocator> allocator;
	ComPtr<ID3D12GraphicsCommandList2> commandList;

	if (!mCommandAllocatorQueue.empty() && mFence.Handle()->GetCompletedValue() >= mCommandAllocatorQueue.front().fenceValue)
	{
		allocator = mCommandAllocatorQueue.front().allocator;
		mCommandAllocatorQueue.pop();
		D3D12Utils::ThrowIfFailed(allocator->Reset());
	}
	else
	{
		allocator = CreateCommandAllocator();
	}

	if (!mCommandListQueue.empty())
	{
		commandList = mCommandListQueue.front();
		mCommandListQueue.pop();
		D3D12Utils::ThrowIfFailed(commandList->Reset(allocator.Get(), nullptr));
	}
	else
	{
		commandList = CreateCommandList(allocator);
	}

	D3D12Utils::ThrowIfFailed(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), allocator.Get()));

	return commandList;
}

std::uint64_t D3D12CommandQueue::ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	commandList->Close();

	ID3D12CommandAllocator* allocator;
	UINT dataSize = sizeof(allocator);
	D3D12Utils::ThrowIfFailed(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &allocator));

	ID3D12CommandList* const lists[] = { commandList.Get() };

	Handle()->ExecuteCommandLists(1, lists);
	std::uint64_t fenceValue = mFence.Signal(*this);

	mCommandAllocatorQueue.emplace(CommandAllocatorEntry{ fenceValue, allocator });
	mCommandListQueue.push(commandList);

	allocator->Release();
	return fenceValue;
}

ComPtr<ID3D12CommandAllocator> D3D12CommandQueue::CreateCommandAllocator()
{
	ComPtr<ID3D12CommandAllocator> allocator;
	D3D12Utils::ThrowIfFailed(mDevice.Handle()->CreateCommandAllocator(mType, IID_PPV_ARGS(&allocator)));
	return allocator;
}

ComPtr<ID3D12GraphicsCommandList2> D3D12CommandQueue::CreateCommandList(ComPtr<ID3D12CommandAllocator>& allocator)
{
	ComPtr<ID3D12GraphicsCommandList2> commandList;
	D3D12Utils::ThrowIfFailed(mDevice.Handle()->CreateCommandList(0, mType, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
	return commandList;
}


}