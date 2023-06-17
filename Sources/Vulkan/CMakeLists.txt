cmake_minimum_required(VERSION 3.21)
project(Vulkan)

# Create target
file(GLOB SOURCES *.h *.cpp)
add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
add_library(Lucid::Vulkan ALIAS ${PROJECT_NAME})

# Find packages
find_package(imgui CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS log)

# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    Vulkan::Vulkan 
    Utils
    unofficial::shaderc::shaderc
    Lucid::Core
    glfw
    Boost::log
    imgui::imgui
)

# Set compile options
SetMaxWarningLevel(${PROJECT_NAME})
SetWindowsVersion(${PROJECT_NAME})
SetLucidVersion(${PROJECT_NAME})

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_WIN32_KHR NOMINMAX)
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_METAL_EXT)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_XCB_KHR)

    find_package(X11 REQUIRED)
    find_package(X11_XCB REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES} ${X11_XCB_LIBRARIES})
endif()
