#pragma once

#include <array>
#include <cstdint>
#include <string>

struct Defaults
{
    inline static const std::uint32_t Width = 1600;
    inline static const std::uint32_t Height = 900;
    inline static const std::string ApplicationName = "Lucid";
    inline static const std::string Version = LUCID_RENDER_VERSION;
    inline static const std::string EngineName = "LucidEngine";
    inline static const std::array<float, 4> BackgroundColor = { 0.05f, 0.05f, 0.05f, 1.0f };
    inline static const std::array<float, 3> AmbientColor = { 1.0f, 1.0f, 1.0f };
    inline static const std::uint32_t MaxFramesInFlight = 3;
    inline static const bool DrawSkybox = false;

#ifndef NDEBUG
    inline static const bool EnableValidationLayers = true;
#else
    inline static const bool EnableValidationLayers = false;
#endif

    enum class Platforms
    {
        MacOS,
        Linux,
        Windows
    };

#if defined(_WIN32)
    inline static const Platforms Platform = Platforms::Windows;
#elif defined(__linux__)
    inline static const Platforms Platform = Platforms::Linux;
#elif defined(__APPLE__)
    inline static const Platforms Platform = Platforms::MacOS;
#endif
};
