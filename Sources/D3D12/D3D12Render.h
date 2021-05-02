#pragma once

#include <wrl.h>
#include <cstdint>
#include <Core/Interfaces.h>
#include <Core/Scene.h>
#include <D3D12/D3D12Adapter.h>
#include <D3D12/D3D12Device.h>
#include <D3D12/D3D12CommandQueue.h>
#include <D3D12/D3D12CommandAllocator.h>
#include <D3D12/D3D12Swapchain.h>
#include <D3D12/D3D12DescriptorHeap.h>
#include <D3D12/D3D12Fence.h>

namespace Lucid::D3D12
{

using namespace Microsoft::WRL;

class D3D12Render : public Core::IRender
{
public:
	D3D12Render(const Core::IWindow& window, const Core::Scene& scene);
	~D3D12Render() override;
	void DrawFrame() override;
	void AddAsset(const Core::Asset& asset) override;

private:
	// D3D12 entities
	std::unique_ptr<D3D12Adapter> mAdapter;
	std::unique_ptr<D3D12Device> mDevice;
	std::unique_ptr<D3D12CommandQueue> mCommandQueue;
	std::unique_ptr<D3D12Swapchain> mSwapchain;
	std::unique_ptr<D3D12DescriptorHeap> mDescriptorHeap;
	std::vector<std::unique_ptr<D3D12CommandAllocator>> mCommandAllocators;
	ComPtr<ID3D12GraphicsCommandList> mCommandList;

	// Synchronization
	std::uint32_t mCurrentFrame = 0;
	std::unique_ptr<D3D12Fence> mFence;
	std::vector<std::uint64_t> mFrameFenceValues;

	const Core::IWindow* mWindow = nullptr;
	const Core::Scene& mScene;
};

}