cmake_minimum_required(VERSION 3.10)
project(D3D12)

set(HEADERS
    D3D12Utils.cpp
    D3D12Render.cpp
    D3D12Adapter.cpp
    D3D12Entity.cpp
    D3D12Device.cpp
    D3D12CommandQueue.cpp
    D3D12Swapchain.cpp
    D3D12DescriptorHeap.cpp
    D3D12CommandAllocator.cpp)

set(SOURCES
    D3D12Utils.h
    D3D12Render.h
    D3D12Adapter.h
    D3D12Entity.h
    D3D12Device.h
    D3D12CommandQueue.h
    D3D12Swapchain.h
    D3D12DescriptorHeap.h
    d3dx12.h
    D3D12CommandAllocator.h)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
add_library(Lucid::D3D12 ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Utils
    d3d12.lib
    dxgi.lib
    Lucid::Core)
