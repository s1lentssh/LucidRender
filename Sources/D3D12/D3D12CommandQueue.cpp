#include <D3D12/D3D12CommandQueue.h>

namespace Lucid::D3D12
{

D3D12CommandQueue::D3D12CommandQueue(D3D12Device& device, D3D12_COMMAND_LIST_TYPE type)
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	device.Handle()->CreateCommandQueue(&desc, IID_PPV_ARGS(&mHandle));
}

}