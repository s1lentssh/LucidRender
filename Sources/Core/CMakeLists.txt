cmake_minimum_required(VERSION 3.10)
project(Core)

# Create target
file(GLOB SOURCES *.cpp *.h)
add_library(${PROJECT_NAME} ${SOURCES})
add_library(Lucid::Core ALIAS ${PROJECT_NAME})

# Find packages
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

# Link Libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    glm::glm
    Vulkan
)

# Set compile options
SetMaxWarningLevel(${PROJECT_NAME})
SetLucidVersion(${PROJECT_NAME})
