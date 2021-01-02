#pragma once

namespace Lucid::Core
{

template<typename T>
struct Vector2d 
{
    T x;
    T y;

    bool IsZero() const
    {
        return x == 0 && y == 0;
    }
};

}
