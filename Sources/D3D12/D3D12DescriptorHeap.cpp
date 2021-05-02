#include <D3D12/D3D12DescriptorHeap.h>
#include <D3D12/D3D12Utils.h>

namespace Lucid::D3D12
{

D3D12DescriptorHeap::D3D12DescriptorHeap(D3D12Device& device, std::uint32_t count, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = type;
	desc.NumDescriptors = count;

	D3D12Utils::ThrowIfFailed(device.Handle()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mHandle)));
}

}