#include "Window.h"

#include <Core/Engine.h>
#include <Utils/Defaults.hpp>
#include <Utils/Files.h>
#include <Utils/Logger.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

static void
ShowStartupInfo()
{
    LoggerInfo << "Version " << Defaults::Version;

    std::string title = R"(
  █░░ █░█ █▀▀ █ █▀▄   █▀█ █▀▀ █▄░█ █▀▄ █▀▀ █▀█
  █▄▄ █▄█ █▄▄ █ █▄▀   █▀▄ ██▄ █░▀█ █▄▀ ██▄ █▀▄
    )";
    LoggerPlain << title;

#ifdef _WIN32
    SetConsoleTitle((Defaults::ApplicationName + " Console").c_str());
#endif

#ifdef __linux__
    std::cout << "\033]0;" << Defaults::ApplicationName << " Console"
              << "\007";
#endif
}

auto
main(std::int32_t argc, const char** argv) -> int
try
{
    ShowStartupInfo();

    boost::program_options::options_description description("Lucid options");
    description.add_options()("help", "Show help message")(
        "scene", boost::program_options::value<std::string>(), "Path to scene to load");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vm);
    boost::program_options::notify(vm);

    if (vm.contains("help"))
    {
        LoggerPlain << description;
        return EXIT_SUCCESS;
    }

    if (!vm.contains("scene"))
    {
        LoggerError << "Scene not provided";
        LoggerPlain << description;
        return EXIT_FAILURE;
    }

    std::unique_ptr<Lucid::Core::IWindow> window = std::make_unique<Lucid::Window>();
    window->SetIcon("Resources/Icons/AppIcon.png");

    std::unique_ptr<Lucid::Core::Engine> engine
        = std::make_unique<Lucid::Core::Engine>(*window.get(), Lucid::Core::Engine::API::Vulkan);

    Lucid::Core::Scene::NodePtr scene = Lucid::Files::LoadModel(vm.at("scene").as<std::string>());
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
