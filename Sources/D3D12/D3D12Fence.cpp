#include <D3D12/D3D12Fence.h>
#include <D3D12/D3D12Utils.h>
#include <D3D12/D3D12CommandQueue.h>
#include <chrono>
#include <cassert>

namespace Lucid::D3D12
{

D3D12Fence::D3D12Fence(D3D12Device& device)
{
	D3D12Utils::ThrowIfFailed(device.Handle()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mHandle)));

	// Create event
	mFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(mFenceEvent && "Failed to create fence event.");
}

D3D12Fence::~D3D12Fence()
{
	::CloseHandle(mFenceEvent);
}

std::uint64_t D3D12Fence::Signal(D3D12CommandQueue& queue)
{
	std::uint64_t valueForSignal = ++mFenceValue;
	D3D12Utils::ThrowIfFailed(queue.Handle()->Signal(Handle().Get(), valueForSignal));
	return valueForSignal;
}

void D3D12Fence::Wait(std::uint64_t value)
{
	if (Handle()->GetCompletedValue() < value)
	{
		D3D12Utils::ThrowIfFailed(Handle()->SetEventOnCompletion(value, mFenceEvent));
		::WaitForSingleObject(mFenceEvent, static_cast<DWORD>(std::chrono::milliseconds::max().count()));
	}
}

void D3D12Fence::Flush(D3D12CommandQueue& queue)
{
	std::uint64_t valueForSignal = Signal(queue);
	Wait(valueForSignal);
}

}