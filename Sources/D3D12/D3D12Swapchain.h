#pragma once

#include <D3D12/D3D12Entity.h>
#include <wrl.h>
#include <d3d12.h>
#include <vector>

#include <D3D12/D3D12Adapter.h>
#include <D3D12/D3D12CommandQueue.h>
#include <D3D12/D3D12Device.h>
#include <D3D12/D3D12DescriptorHeap.h>
#include <Core/Interfaces.h>

namespace Lucid::D3D12
{

using namespace Microsoft::WRL;

class D3D12Swapchain : public D3D12Entity<ComPtr<IDXGISwapChain4>>
{
public:
	D3D12Swapchain(D3D12Device& device, const D3D12CommandQueue& commandQueue, const Core::IWindow& window);
	void CreateRenderTargetViews(D3D12DescriptorHeap& heap);

private:
	D3D12Device& mDevice;
	std::vector<ComPtr<ID3D12Resource>> mBackBuffers;
};

}