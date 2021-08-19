#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <D3D12/D3D12Entity.h>
#include <D3D12/D3D12Device.h>

namespace Lucid::D3D12
{

using namespace Microsoft::WRL;

class D3D12CommandAllocator : public D3D12Entity<ComPtr<ID3D12CommandAllocator>>
{
public:
	D3D12CommandAllocator(D3D12Device& device, D3D12_COMMAND_LIST_TYPE type);

private:
	D3D12Device& mDevice;
	D3D12_COMMAND_LIST_TYPE mType;
};

}