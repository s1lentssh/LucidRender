#pragma once

namespace Lucid::D3D12
{

template<typename HandleType>
class D3D12Entity
{
public:
	[[nodiscard]] const HandleType& Handle() const
	{
		return mHandle;
	}

protected:
	HandleType mHandle;
};

}
