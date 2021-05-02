#pragma once

#include <D3D12/D3D12Entity.h>
#include <wrl.h>
#include <dxgi1_6.h>

namespace Lucid::D3D12
{

using namespace Microsoft::WRL;

class D3D12Adapter : public D3D12Entity<ComPtr<IDXGIAdapter4>>
{
public:
	D3D12Adapter();
};

}