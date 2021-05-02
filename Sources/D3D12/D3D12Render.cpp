#include <D3D12/D3D12Render.h>
#include <D3D12/D3D12Utils.h>
#include <Utils/Defaults.hpp>
#include <d3d12sdklayers.h>

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
	// mCommandQueue = std::make_unique<D3D12CommandQueue>(*mAdapter.get());
}

void D3D12Render::DrawFrame()
{}

void D3D12Render::AddAsset(const Core::Asset & asset)
{}

}