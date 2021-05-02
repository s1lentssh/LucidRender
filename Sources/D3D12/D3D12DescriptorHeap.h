#pragma once

#include <D3D12/D3D12Entity.h>
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>

#include <D3D12/D3D12Device.h>

namespace Lucid::D3D12
{

using namespace Microsoft::WRL;

class D3D12DescriptorHeap : public D3D12Entity<ComPtr<ID3D12DescriptorHeap>>
{
public:
	D3D12DescriptorHeap(D3D12Device& device, std::uint32_t count, D3D12_DESCRIPTOR_HEAP_TYPE type);
};

}