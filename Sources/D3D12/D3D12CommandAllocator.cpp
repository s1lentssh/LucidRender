#include <D3D12/D3D12CommandAllocator.h>
#include <D3D12/D3D12Utils.h>

namespace Lucid::D3D12
{

D3D12CommandAllocator::D3D12CommandAllocator(D3D12Device& device, D3D12_COMMAND_LIST_TYPE type)
	: mDevice(device)
	, mType(type)
{
	D3D12Utils::ThrowIfFailed(device.Handle()->CreateCommandAllocator(type, IID_PPV_ARGS(&mHandle)));
}

}
