#pragma once

#include <memory>
#include <set>

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
    void SetRootNode(const SceneNodePtr& node);
    bool ShouldClose() const;

private:
    void ProcessInput(float time);
    void ProcessDirtyNodes();

    std::shared_ptr<Lucid::Core::Scene> mScene;
    std::unique_ptr<Lucid::Core::IRender> mRender;

    std::set<std::size_t> mDirtyNodes;
};

} // namespace Lucid::Core
