#pragma once

#include <cstdint>
#include <string>

struct Defaults
{
    inline static const std::uint32_t Width = 800;
    inline static const std::uint32_t Height = 600;
    inline static const std::string ApplicationName = "LucidRender";
    inline static const std::string EngineName = "LucidEngine";

#ifndef NDEBUG
    inline static const bool EnableValidationLayers = true;
#else
    inline static const bool EnableValidationLayers = false;
#endif
};