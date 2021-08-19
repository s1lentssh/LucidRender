#include <D3D12/D3D12Render.h>
#include <D3D12/D3D12Utils.h>
#include <Utils/Defaults.hpp>
#include <d3d12sdklayers.h>
#include <D3D12/d3dx12.h>
#include <d3dcompiler.h>

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
	mCommandQueue = std::make_unique<D3D12CommandQueue>(*mDevice.get(), D3D12_COMMAND_LIST_TYPE_COPY);

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
	// mCommandList = mCommandQueue->CreateCommandList(D3D12_COMMAND_LIST_TYPE_COPY);

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

void D3D12Render::LoadContent()
{
	auto commandQueue = mCommandQueue->Handle();
	auto commandList = mCommandQueue->GetCommandList();

	ComPtr<ID3D12Resource> intermediateVertexBuffer;
	UpdateBufferResource(
		commandList.Get(),
		&mVertexBuffer,
		&intermediateVertexBuffer,
		_countof(gVertices),
		sizeof(VertexPositionColor),
		gVertices);

	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.SizeInBytes = sizeof(gVertices);
	mVertexBufferView.StrideInBytes = sizeof(VertexPositionColor);

	ComPtr<ID3D12Resource> intermediateIndexBuffer;
	UpdateBufferResource(
		commandList.Get(),
		&mIndexBuffer,
		&intermediateIndexBuffer,
		_countof(gIndicies),
		sizeof(WORD),
		gIndicies);

	mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
	mIndexBufferView.SizeInBytes = sizeof(gIndicies);
	mIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	D3D12Utils::ThrowIfFailed(mDevice->Handle()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mDepthBufferHeap)));

	ComPtr<ID3DBlob> vertexShaderBlob;
	D3D12Utils::ThrowIfFailed(D3DReadFileToBlob(L"E:\Work\LucidRender\Resources\Shaders\Vertex.fxc", &vertexShaderBlob));

	ComPtr<ID3DBlob> pixelShaderBlob;
	D3D12Utils::ThrowIfFailed(D3DReadFileToBlob(L"E:\Work\LucidRender\Resources\Shaders\Pixel.fxc", &pixelShaderBlob));

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(mDevice->Handle()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	D3D12_ROOT_SIGNATURE_FLAGS signatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	CD3DX12_ROOT_PARAMETER1 rootParameters[1];
	rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC signatureDesc = {};
	signatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, signatureFlags);

	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;
	D3D12Utils::ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&signatureDesc, featureData.HighestVersion, 
		&signatureBlob, &errorBlob));
	D3D12Utils::ThrowIfFailed(mDevice->Handle()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), 
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));
}

void D3D12Render::UpdateBufferResource(ComPtr<ID3D12GraphicsCommandList2> commandList, ID3D12Resource * *destination, ID3D12Resource * *intermediate, std::size_t numElements, std::size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags)
{
	std::size_t bufferSize = numElements * elementSize;
	D3D12Utils::ThrowIfFailed(mDevice->Handle()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(destination)
	));

	if (bufferData)
	{
		D3D12Utils::ThrowIfFailed(mDevice->Handle()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(intermediate)
		));

		D3D12_SUBRESOURCE_DATA subresource = {};
		subresource.pData = bufferData;
		subresource.RowPitch = bufferSize;
		subresource.SlicePitch = bufferSize;

		UpdateSubresources(commandList.Get(), *destination, *intermediate, 0, 0, 1, &subresource);
	}
}

}