cmake_minimum_required(VERSION 3.21)
project(Standalone)

# Create target
file(GLOB SOURCES *.cpp *.h)
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Standalone")

# Link libraries
target_link_libraries(${PROJECT_NAME} Lucid::Lucid)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_EXPOSE_NATIVE_WIN32)
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_EXPOSE_NATIVE_COCOA)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework QuartzCore")
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_EXPOSE_NATIVE_X11)
endif()

# Set compile options
SetMaxWarningLevel(${PROJECT_NAME})
SetWindowsVersion(${PROJECT_NAME})
SetLucidVersion(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES 
    XCODE_GENERATE_SCHEME TRUE 
    XCODE_SCHEME_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(DIRECTORY ${RESOURCES_DIR} DESTINATION .)
