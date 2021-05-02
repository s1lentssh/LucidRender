#pragma once

#include <D3D12/D3D12Entity.h>
#include <wrl.h>
#include <d3d12.h>

#include <D3D12/D3D12Adapter.h>

namespace Lucid::D3D12
{

using namespace Microsoft::WRL;

class D3D12Device : public D3D12Entity<ComPtr<ID3D12Device2>>
{
public:
	D3D12Device(const D3D12Adapter& adapter);
};

}