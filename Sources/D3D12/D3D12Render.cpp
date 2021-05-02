#include <D3D12/D3D12Render.h>
#include <D3D12/D3D12Utils.h>
#include <Utils/Defaults.hpp>
#include <d3d12sdklayers.h>
#include <D3D12/d3dx12.h>

namespace Lucid::D3D12
{

D3D12Render::D3D12Render(const Core::IWindow& window, const Core::Scene& scene)
	: mWindow(&window), mScene(scene)
{
	if constexpr (Defaults::EnableValidationLayers)
	{
		ComPtr<ID3D12Debug> debugInterface;
		D3D12Utils::ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
		debugInterface->EnableDebugLayer();
	}

	// Create adapter
	mAdapter = std::make_unique<D3D12Adapter>();
	
	// Create device
	mDevice = std::make_unique<D3D12Device>(*mAdapter.get());

	// Create command queue
	mCommandQueue = std::make_unique<D3D12CommandQueue>(*mDevice.get(), D3D12_COMMAND_LIST_TYPE_DIRECT);

	// Create swapchain
	mSwapchain = std::make_unique<D3D12Swapchain>(*mDevice.get(), *mCommandQueue.get(), window);
	mCurrentFrame = mSwapchain->Handle()->GetCurrentBackBufferIndex();

	// Create descriptor heap
	mDescriptorHeap = std::make_unique<D3D12DescriptorHeap>(*mDevice.get(), Defaults::MaxFramesInFlight, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mSwapchain->CreateRenderTargetViews(*mDescriptorHeap.get());

	// Create command allocators
	for (std::size_t i = 0; i < Defaults::MaxFramesInFlight; i++)
	{
		mCommandAllocators.push_back(std::make_unique<D3D12CommandAllocator>(*mDevice.get(), D3D12_COMMAND_LIST_TYPE_DIRECT));
	}

	// Create command list
	mCommandList = mCommandAllocators.back()->CreateCommandList();

	// Create fence
	mFence = std::make_unique<D3D12Fence>(*mDevice.get());
	mFrameFenceValues.resize(Defaults::MaxFramesInFlight);
	std::fill(mFrameFenceValues.begin(), mFrameFenceValues.end(), 0);
}

D3D12Render::~D3D12Render()
{
	mFence->Flush(*mCommandQueue.get());
}

void D3D12Render::DrawFrame()
{
	auto& allocator = mCommandAllocators.at(mCurrentFrame);
	auto backBuffer = mSwapchain->GetBackBuffer(mCurrentFrame);

	allocator->Handle()->Reset();
	mCommandList->Reset(allocator->Handle().Get(), nullptr);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffer.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	mCommandList->ResourceBarrier(1, &barrier);

	UINT descriptorSize = mDevice->Handle()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
		mDescriptorHeap->Handle()->GetCPUDescriptorHandleForHeapStart(), 
		mCurrentFrame,
		descriptorSize
	);

	mCommandList->ClearRenderTargetView(rtv, Defaults::BackgroundColor.data(), 0, nullptr);

	// Present
	CD3DX12_RESOURCE_BARRIER presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	mCommandList->ResourceBarrier(1, &presentBarrier);
	D3D12Utils::ThrowIfFailed(mCommandList->Close());

	ID3D12CommandList* const commandLists[] = { mCommandList.Get() };
	mCommandQueue->Handle()->ExecuteCommandLists(_countof(commandLists), commandLists);

	D3D12Utils::ThrowIfFailed(mSwapchain->Handle()->Present(1, 0));

	mFrameFenceValues.at(mCurrentFrame) = mFence->Signal(*mCommandQueue.get());
	mCurrentFrame = mSwapchain->Handle()->GetCurrentBackBufferIndex();
	mFence->Wait(mFrameFenceValues.at(mCurrentFrame));
}

void D3D12Render::AddAsset(const Core::Asset & asset)
{}

}