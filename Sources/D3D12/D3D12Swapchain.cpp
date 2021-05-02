#include <D3D12/D3D12Swapchain.h>
#include <Utils/Defaults.hpp>
#include <D3D12/D3D12Utils.h>
#include <dxgi.h>
#include <D3D12/d3dx12.h>

namespace Lucid::D3D12
{

D3D12Swapchain::D3D12Swapchain(D3D12Device& device, const D3D12CommandQueue& commandQueue, const Core::IWindow& window)
	: mDevice(device)
{
	// Create factory
	ComPtr<IDXGIFactory4> factory4;
	UINT flags = 0;

	if constexpr (Defaults::EnableValidationLayers)
	{
		flags = DXGI_CREATE_FACTORY_DEBUG;
	}

	D3D12Utils::ThrowIfFailed(CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory4)));

	// Create swapchain
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = Defaults::Width;
	desc.Height = Defaults::Height;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Stereo = FALSE;
	desc.SampleDesc = { 1, 0 };
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = Defaults::MaxFramesInFlight;
	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.Flags = D3D12Utils::IsTearingAllowed() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	ComPtr<IDXGISwapChain1> swapchain1;

	D3D12Utils::ThrowIfFailed(factory4->CreateSwapChainForHwnd(
		commandQueue.Handle().Get(),
		reinterpret_cast<HWND>(window.Handle()),
		&desc,
		nullptr,
		nullptr,
		&swapchain1)
	);

	D3D12Utils::ThrowIfFailed(factory4->MakeWindowAssociation(reinterpret_cast<HWND>(window.Handle()), DXGI_MWA_NO_ALT_ENTER));
	D3D12Utils::ThrowIfFailed(swapchain1.As(&mHandle));
}

void D3D12Swapchain::CreateRenderTargetViews(D3D12DescriptorHeap& heap)
{
	UINT descriptorSize = mDevice.Handle()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(heap.Handle()->GetCPUDescriptorHandleForHeapStart());

	for (std::uint32_t i = 0; i < Defaults::MaxFramesInFlight; i++)
	{
		ComPtr<ID3D12Resource> backBuffer;
		D3D12Utils::ThrowIfFailed(Handle()->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

		mDevice.Handle()->CreateRenderTargetView(backBuffer.Get(), nullptr, handle);
		mBackBuffers.push_back(backBuffer);

		handle.Offset(descriptorSize);
	}
}

}
