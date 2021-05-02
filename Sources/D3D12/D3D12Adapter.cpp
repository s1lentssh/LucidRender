#include <D3D12/D3D12Adapter.h>
#include <D3D12/D3D12Utils.h>
#include <Utils/Defaults.hpp>
#include <d3d12.h>

namespace Lucid::D3D12
{

D3D12Adapter::D3D12Adapter()
{
	ComPtr<IDXGIFactory4> factory;
	UINT flags = 0;

	if constexpr (Defaults::EnableValidationLayers)
	{
		flags = DXGI_CREATE_FACTORY_DEBUG;
	}

	D3D12Utils::ThrowIfFailed(CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory)));

	ComPtr<IDXGIAdapter1> adapter1;
	ComPtr<IDXGIAdapter4> adapter4;

	SIZE_T maxVideoMemory = 0;
	for (UINT i = 0; factory->EnumAdapters1(i, &adapter1) != DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC1 desc1;
		adapter1->GetDesc1(&desc1);

		bool notSoftware = desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE;
		bool canCreate = SUCCEEDED(D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr));
		bool maxMemory = desc1.DedicatedVideoMemory > maxVideoMemory;

		if (notSoftware && canCreate && maxMemory)
		{
			maxVideoMemory = desc1.DedicatedVideoMemory;
			D3D12Utils::ThrowIfFailed(adapter1.As(&adapter4));
		}
	}

	mHandle = adapter4;
}

}