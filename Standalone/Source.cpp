#include "Window.h"

#include <Core/Engine.h>
#include <Utils/Defaults.hpp>
#include <Utils/Files.h>
#include <Utils/Logger.hpp>

auto
main() -> int
try
{
    LoggerInfo << "Version " << Defaults::Version;

    std::string title = R"(
  █░░ █░█ █▀▀ █ █▀▄   █▀█ █▀▀ █▄░█ █▀▄ █▀▀ █▀█
  █▄▄ █▄█ █▄▄ █ █▄▀   █▀▄ ██▄ █░▀█ █▄▀ ██▄ █▀▄
    )";
    std::cout << title << '\n';

#ifdef _WIN32
    SetConsoleTitle((Defaults::ApplicationName + " Console").c_str());
#endif

#ifdef __linux__
    std::cout << "\033]0;" << Defaults::ApplicationName << " Console"
              << "\007";
#endif

    std::unique_ptr<Lucid::Core::IWindow> window = std::make_unique<Lucid::Window>();
    window->SetIcon("Resources/Icons/AppIcon.png");

    std::unique_ptr<Lucid::Core::Engine> engine
        = std::make_unique<Lucid::Core::Engine>(*window.get(), Lucid::Core::Engine::API::Vulkan);

    Lucid::Core::SceneNodePtr scene = Lucid::Files::LoadModel("/home/s1lentssh/Work/glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf");
    engine->SetRootNode(scene);

    float lastTime = static_cast<float>(glfwGetTime());

    while (!window->ShouldClose() && !engine->ShouldClose())
    {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;

        window->PollEvents();
        engine->Update(deltaTime);

        lastTime = currentTime;
    }

    return EXIT_SUCCESS;
}
catch (const std::runtime_error& ex)
{
    LoggerError << ex.what();
    return EXIT_FAILURE;
}
catch (const std::exception& ex)
{
    LoggerError << ex.what();
    return EXIT_FAILURE;
}
catch (...)
{
    LoggerError << "Unknown reason";
    return EXIT_FAILURE;
}
