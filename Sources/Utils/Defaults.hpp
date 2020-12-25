#pragma once

#include <cstdint>
#include <string>
#include <array>

struct Defaults
{
    inline static const std::uint32_t Width = 1200;
    inline static const std::uint32_t Height = 600;
    inline static const std::string ApplicationName = "Lucid";
    inline static const std::string Version = "0.1 pre-alpha";
    inline static const std::string EngineName = "LucidEngine";
    inline static const std::array<float, 4> BackgroundColor = { 0.003f, 0.003f, 0.003f, 1.0f };
    inline static const std::uint32_t MaxFramesInFlight = 2;

#ifndef NDEBUG
    inline static const bool EnableValidationLayers = true;
#else
    inline static const bool EnableValidationLayers = false;
#endif
};