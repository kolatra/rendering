cmake_minimum_required(VERSION 4.3)
project(engine_v29090909 LANGUAGES C CXX OBJC OBJCXX)

option(ENGINE_ENABLE_VULKAN "Find and link Vulkan backend dependencies" OFF)
option(BUILD_SHARED_LIBS "Build GLFW and Assimp as shared libraries" OFF)

add_subdirectory(external/glfw)
add_subdirectory(external/spdlog)


if(APPLE)
    message(STATUS "Configuring for macOS")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")

    find_library(METAL_FRAMEWORK Metal REQUIRED)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
    find_library(APPKIT_FRAMEWORK AppKit REQUIRED)
    
    
endif()

# Assimp is included as a submodule so the project does not depend on a
# separately installed Assimp package.
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build Assimp tests" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Build Assimp tools" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "Build Assimp samples" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "Install Assimp" FORCE)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/assimp/CMakeLists.txt")
    add_subdirectory(external/assimp)
else()
    message(FATAL_ERROR
        "The Assimp submodule is missing. Initialize external/assimp before configuring the project."
    )
endif()

add_executable(${PROJECT_NAME}
        example-app/example.cpp
        include/engine/platform/IWindow.hpp
        src/engine/platform/WindowGLFW.cpp
        include/engine/platform/WindowGLFW.hpp
        include/engine/core/Application.hpp
        include/engine/graphics/IRenderer.hpp
        src/engine/graphics/opengl/RendererOpenGL.cpp
        include/engine/graphics/opengl/RendererOpenGL.hpp
        src/engine/graphics/IModel.cpp
        include/engine/graphics/opengl/GlModel.hpp
        src/engine/systems/entity/ECS.cpp
        include/engine/systems/entity/ECS.hpp
        external/glad/gl.c
        src/engine/core/Application.cpp
        src/engine/graphics/opengl/GlMesh.cpp
        src/engine/core/Config.cpp
        include/engine/core/Config.hpp
        include/engine/graphics/metal/MetalRenderer.hpp
        include/engine/core/Camera.hpp
	include/engine/platform/WindowNS.hpp
        src/engine/core/Camera.cpp
)

include(CheckCXXSourceCompiles)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
check_cxx_source_compiles("int main() { return 0; }" ENGINE_COMPILER_SUPPORTS_CXX26)

if(ENGINE_COMPILER_SUPPORTS_CXX26)
    set(ENGINE_CXX_STANDARD 26)
    message(STATUS "Using C++26")
else()
    set(ENGINE_CXX_STANDARD 23)
    message(STATUS "Compiler does not support C++26; falling back to C++23")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD ${ENGINE_CXX_STANDARD}
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8 /bigobj)
endif()

if(APPLE)
    target_include_directories(${PROJECT_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/external/metal-cpp
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${METAL_FRAMEWORK}
            ${QUARTZCORE_FRAMEWORK}
            ${APPKIT_FRAMEWORK}
    )

    target_sources(${PROJECT_NAME} PRIVATE
            src/engine/core/MetalApp.mm
            src/engine/graphics/metal/MetalRenderer.cc
            src/engine/platform/WindowMetal.mm
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_EXPOSE_NATIVE_COCOA)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:OBJCXX>:-fobjc-arc>)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/external"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    assimp::assimp
)

if(ENGINE_ENABLE_VULKAN)
    find_package(Vulkan REQUIRED COMPONENTS volk shaderc_combined)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Vulkan::Vulkan
        Vulkan::volk
        Vulkan::shaderc_combined
    )
endif()

if(WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLL dependencies"
    )
endif()
