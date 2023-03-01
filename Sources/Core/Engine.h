#pragma once

#include <memory>

#include <Core/Interfaces.h>
#include <Core/Scene.h>

namespace Lucid::Core
{

class Engine
{
public:
    enum class API
    {
        Vulkan
    };

    Engine(const IWindow& window, API api);
    void Update(float time);
    void AddAsset(const Core::Asset& asset);
    bool ShouldClose() const;

private:
    void ProcessInput(float time);

    std::shared_ptr<Lucid::Core::Scene> mScene;
    std::unique_ptr<Lucid::Core::IRender> mRender;
};

} // namespace Lucid::Core
