#pragma once

#include <D3D12/D3D12Entity.h>
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>

#include <D3D12/D3D12Device.h>

namespace Lucid::D3D12
{

class D3D12CommandQueue;

using namespace Microsoft::WRL;

class D3D12Fence : public D3D12Entity<ComPtr<ID3D12Fence>>
{
public:
	D3D12Fence(D3D12Device& device);
	~D3D12Fence();
	std::uint64_t Signal(D3D12CommandQueue& queue);
	void Wait(std::uint64_t value);
	void Flush(D3D12CommandQueue& queue);
	std::uint64_t Value() { return mFenceValue; }

private:
	HANDLE mFenceEvent = nullptr;
	uint64_t mFenceValue = 0;
};

}