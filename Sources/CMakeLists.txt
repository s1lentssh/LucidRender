add_subdirectory(Utils)
add_subdirectory(Vulkan)
add_subdirectory(Application)
add_subdirectory(Core)

if(WIN32)
    add_subdirectory(D3D12)
endif()

