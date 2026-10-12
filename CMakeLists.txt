cmake_minimum_required(VERSION 4.3)
project(engine_v29090909)

set(CMAKE_CXX_STANDARD 26)

set(GLFW_DIR "D:\\Deps\\glfw-3.4.bin.WIN64")
set(GLFW_INCLUDE_DIR ${GLFW_DIR}/include)
set(GLFW_LIB_DIR ${GLFW_DIR}/lib-vc2022)

set(FREETYPE_DIR "D:\\Deps\\freetype-2.9")
set(FREETYPE_INCLUDE_DIR ${FREETYPE_DIR}/include)
set(FREETYPE_LIB_DIR ${FREETYPE_DIR}/objs)

set(SPDLOG_DIR "C:\\Users\\tlouk\\Programming\\spdlog")
set(SPDLOG_INCLUDE_DIR ${SPDLOG_DIR}/include)
set(SPDLOG_LIB_DIR ${SPDLOG_DIR}/build/Debug)

set(ASSIMP_DIR "C:\\Users\\tlouk\\git\\assimp")
set(ASSIMP_INCLUDE_DIR ${ASSIMP_DIR}/include)
set(ASSIMP_LIB_DIR "C:\\Users\\tlouk\\git\\assimp\\lib\\Debug")
set(ASSIMP_BIN_DIR "C:\\Users\\tlouk\\git\\assimp\\bin\\Debug")

if (MSVC)
    add_compile_options(/utf-8 /bigobj)
endif ()

find_package(Vulkan REQUIRED COMPONENTS volk shaderc_combined)

add_executable(engine_v29090909 src/main.cpp
        src/engine/platform/IWindow.hpp
        src/engine/platform/glfw/WindowGLFW.cpp
        src/engine/platform/glfw/WindowGLFW.hpp
        src/engine/Application.cpp
        src/engine/Application.hpp
        src/engine/graphics/IRenderer.hpp
        src/engine/graphics/opengl/RendererOpenGL.cpp
        src/engine/graphics/opengl/RendererOpenGL.hpp
        include/glad/gl.c
        src/engine/graphics/Model.cpp
        src/engine/graphics/Model.hpp
        src/engine/objects/Penger.cpp
        src/engine/objects/Penger.hpp
)

target_include_directories(${PROJECT_NAME}
        PRIVATE ${GLFW_DIR}/include
        PRIVATE ${FREETYPE_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${FREETYPE_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
        ${SPDLOG_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR}
)

target_link_directories(${PROJECT_NAME}
        PRIVATE ${GLFW_LIB_DIR}
        PRIVATE ${FREETYPE_LIB_DIR}
        ${SPDLOG_LIB_DIR}
        ${ASSIMP_LIB_DIR}
        ${ASSIMP_BIN_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw3
        freetype
        assimp-vc145-mtd
        Vulkan::Vulkan
        Vulkan::volk
        Vulkan::shaderc_combined
)
