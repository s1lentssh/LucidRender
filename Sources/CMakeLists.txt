project(Lucid)

add_subdirectory(Utils)
add_subdirectory(Vulkan)
add_subdirectory(Core)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Empty.cpp "int main() { return 0; }")
add_library(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/Empty.cpp)
add_library(Lucid::Lucid ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} INTERFACE
    Lucid::Core
    Lucid::Utils
    Lucid::Vulkan
)
