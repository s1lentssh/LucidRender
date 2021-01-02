#include "Window.h"
#include <Utils/Logger.hpp>
#include <Utils/Defaults.hpp>
#include <Core/Engine.h>

auto main() -> int try
{
    Logger::Info("Version {}", Defaults::Version);

    std::string title = R"(
    ___                            __                              __                  
   /\_ \                    __    /\ \                            /\ \                 
   \//\ \    __  __    ___ /\_\   \_\ \      _ __    __    ___    \_\ \     __   _ __  
     \ \ \  /\ \/\ \  /'___\/\ \  /'_` \    /\`'__\/'__`\/' _ `\  /'_` \  /'__`\/\`'__\
      \_\ \_\ \ \_\ \/\ \__/\ \ \/\ \L\ \   \ \ \//\  __//\ \/\ \/\ \L\ \/\  __/\ \ \/ 
      /\____\\ \____/\ \____\\ \_\ \___,_\   \ \_\\ \____\ \_\ \_\ \___,_\ \____\\ \_\ 
      \/____/ \/___/  \/____/ \/_/\/__,_ /    \/_/ \/____/\/_/\/_/\/__,_ /\/____/ \/_/ 
   
)";
    std::cout << title << '\n';

#ifdef _WIN32
    SetConsoleTitle((Defaults::ApplicationName + " Console").c_str());
#endif

#ifdef __linux__
    std::cout << "\033]0;" << Defaults::ApplicationName << " Console" << "\007";
#endif

    std::unique_ptr<Lucid::Core::IWindow> window = std::make_unique<Lucid::Window>();
    window->SetIcon("Resources/Icons/AppIcon.png");

    std::unique_ptr<Lucid::Core::Engine> engine = std::make_unique<Lucid::Core::Engine>(*window.get());

    float lastTime = glfwGetTime();

    while (!window->ShouldClose())
    {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;

        window->PollEvents();
        engine->Update(deltaTime);

        lastTime = currentTime;
    }

    return EXIT_SUCCESS;
}
catch (const std::exception & ex)
{
    Logger::Error(ex.what());
    return EXIT_FAILURE;
}