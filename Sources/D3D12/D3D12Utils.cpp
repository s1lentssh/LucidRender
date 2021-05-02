#include <D3D12/D3D12Utils.h>
#include <stdexcept>
#include <system_error>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace Lucid::D3D12
{

void D3D12Utils::ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		std::string message = std::system_category().message(hr);
		throw std::runtime_error(message);
	}
}

bool D3D12Utils::IsTearingAllowed()
{
	ComPtr<IDXGIFactory4> factory4;
	BOOL allowTearing = FALSE;

	if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
	{
		ComPtr<IDXGIFactory5> factory5;
		if (SUCCEEDED(factory4.As(&factory5)))
		{
			if (FAILED(factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))))
			{
				allowTearing = FALSE;
			}
		}
	}

	return allowTearing == TRUE;
}

}