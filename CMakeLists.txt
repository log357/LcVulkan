cmake_minimum_required(VERSION 3.20)
project(LcVulkan)
set(CMAKE_CXX_STANDARD 23)

set(SOURCES
    main.cpp
    LcVulkan.cpp
    LcCamera.cpp
    ./ImGui/imgui.cpp
    ./ImGui/imgui_draw.cpp
    ./ImGui/imgui_widgets.cpp
    ./ImGui/imgui_tables.cpp
    ./ImGui/imgui_demo.cpp
    ./ImGui/imgui_impl_glfw.cpp
    ./ImGui/imgui_impl_vulkan.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

set(VULKAN_SDK $ENV{VULKAN_SDK})
target_include_directories(${PROJECT_NAME} PRIVATE ${VULKAN_SDK}/Include)
target_link_directories(${PROJECT_NAME} PRIVATE ${VULKAN_SDK}/Lib)

target_include_directories(${PROJECT_NAME} PRIVATE ./ImGui)

target_link_libraries(${PROJECT_NAME}
    glfw3dll
    vulkan-1
    gdi32
)

