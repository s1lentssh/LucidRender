#pragma once

#include <wrl.h>
#include <cstdint>
#include <DirectXMath.h>
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

	void LoadContent();

	// Temporary for tutorial
	void TransitionResource(
		ComPtr<ID3D12GraphicsCommandList2> commandList, 
		ComPtr<ID3D12Resource> resource, 
		D3D12_RESOURCE_STATES before,
		D3D12_RESOURCE_STATES after);

	void ClearRTV(
		ComPtr<ID3D12GraphicsCommandList2> commandList,
		D3D12_CPU_DESCRIPTOR_HANDLE rtv);

	void ClearDepth(
		ComPtr<ID3D12GraphicsCommandList2> commandList,
		D3D12_CPU_DESCRIPTOR_HANDLE dsv);

	void UpdateBufferResource(
		ComPtr<ID3D12GraphicsCommandList2> commandList,
		ID3D12Resource** destination,
		ID3D12Resource** intermediate,
		std::size_t numElements,
		std::size_t elementSize,
		const void* bufferData,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

	ComPtr<ID3D12Resource> mVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
	ComPtr<ID3D12Resource> mIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
	ComPtr<ID3D12Resource> mDepthBuffer;
	ComPtr<ID3D12DescriptorHeap> mDepthBufferHeap;
	ComPtr<ID3D12RootSignature> mRootSignature;
	ComPtr<ID3D12PipelineState> mPipelineState;
	D3D12_VIEWPORT mViewport = CD3DX12_VIEWPORT(0.f, 0.f, Defaults::Width, Defaults::Height);
	D3D12_RECT mRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);

	float mFov = 45.0;
	DirectX::CXMMATRIX mModelMatrix;
	DirectX::CXMMATRIX mViewMatrix;
	DirectX::CXMMATRIX mProjectionMatrix;

	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Color;
	};

	inline static const VertexPositionColor gVertices[] = {
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
		{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
		{ DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
		{ DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
		{ DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
		{ DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
		{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
	};

	inline static WORD gIndicies[] =
	{
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		4, 5, 1, 4, 1, 0,
		3, 2, 6, 3, 6, 7,
		1, 5, 6, 1, 6, 2,
		4, 0, 3, 4, 3, 7
	};
};

}