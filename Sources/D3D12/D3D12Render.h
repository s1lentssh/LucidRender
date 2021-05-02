#pragma once

#include <wrl.h>
#include <Core/Interfaces.h>
#include <Core/Scene.h>
#include <D3D12/D3D12Adapter.h>
#include <D3D12/D3D12Device.h>
#include <D3D12/D3D12CommandQueue.h>

namespace Lucid::D3D12
{

using namespace Microsoft::WRL;

class D3D12Render : public Core::IRender
{
public:
	D3D12Render(const Core::IWindow& window, const Core::Scene& scene);
	void DrawFrame() override;
	void AddAsset(const Core::Asset& asset) override;

private:
	std::unique_ptr<D3D12Adapter> mAdapter;
	std::unique_ptr<D3D12Device> mDevice;
	std::unique_ptr<D3D12CommandQueue> mCommandQueue;

	const Core::IWindow* mWindow = nullptr;
	const Core::Scene& mScene;
};

}