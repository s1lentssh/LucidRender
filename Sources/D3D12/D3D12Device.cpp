#include <D3D12/D3D12Device.h>
#include <D3D12/D3D12Utils.h>
#include <Utils/Defaults.hpp>

namespace Lucid::D3D12
{

D3D12Device::D3D12Device(const D3D12Adapter& adapter)
{
	D3D12Utils::ThrowIfFailed(D3D12CreateDevice(adapter.Handle().Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mHandle)));

	if constexpr (Defaults::EnableValidationLayers)
	{
		ComPtr<ID3D12InfoQueue> info;
		if (SUCCEEDED(mHandle.As(&info)))
		{
			D3D12_MESSAGE_SEVERITY severities[] = {
				D3D12_MESSAGE_SEVERITY_CORRUPTION,
				D3D12_MESSAGE_SEVERITY_ERROR,
				D3D12_MESSAGE_SEVERITY_WARNING,
				/* D3D12_MESSAGE_SEVERITY_INFO, */
				/* D3D12_MESSAGE_SEVERITY_MESSAGE, */
			};

			for (D3D12_MESSAGE_SEVERITY severity : severities)
			{
				info->SetBreakOnSeverity(severity, TRUE);
			}

			D3D12_MESSAGE_SEVERITY filteredSeverities[] = { D3D12_MESSAGE_SEVERITY_INFO };
			D3D12_MESSAGE_ID denyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
			};

			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumSeverities = _countof(severities);
			filter.DenyList.pSeverityList = filteredSeverities;
			filter.DenyList.NumIDs = _countof(denyIds);
			filter.DenyList.pIDList = denyIds;

			D3D12Utils::ThrowIfFailed(info->PushStorageFilter(&filter));
		}
	}
}

}