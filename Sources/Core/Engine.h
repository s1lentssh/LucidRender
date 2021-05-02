#pragma once

#include <memory>
#include <Core/Scene.h>
#include <Core/Interfaces.h>

namespace Lucid::Core
{

class Engine
{
public:
	Engine(const IWindow& window);
	void Update(float time);
	void AddAsset(const Core::Asset& asset);
	
private:
	void ProcessInput(float time);

	std::shared_ptr<Lucid::Core::Scene> mScene;
	std::unique_ptr<Lucid::Core::IRender> mRender;
};

}
