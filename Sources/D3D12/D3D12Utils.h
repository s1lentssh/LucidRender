#pragma once

#include <winerror.h>
#include <wrl.h>

namespace Lucid::D3D12
{

using namespace Microsoft::WRL;

class D3D12Utils
{
public:
	static void ThrowIfFailed(HRESULT hr);
	static bool IsTearingAllowed();
};

}