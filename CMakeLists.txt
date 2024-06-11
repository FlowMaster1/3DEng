cmake_minimum_required(VERSION 3.5.0)
project(3DEng VERSION 0.1.0)

set (CMAKE_CXX_SYANDARD 20)

find_package(OpenGL REQUIRED)

set(IMGUI_SOURCES
    libs/imgui/imgui.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/backends/imgui_impl_sdl2.cpp
    libs/imgui/backends/imgui_impl_opengl3.cpp
)

add_subdirectory(libs/SDL2)
add_subdirectory(libs/glm)
add_subdirectory(libs/assimp)

file(GLOB SOURCES "src/*.cpp", "src/*/*.cpp", "src/*/*/*.cpp", "src/*/*/*/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        libs/imgui
        libs/SDL2
        libs/glad
        libs/glm
        libs/stb_image  
        libs/assimp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${OPENGL_gl_LIBRARY}
        SDL2
        glm
        assimp

)