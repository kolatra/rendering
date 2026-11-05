cmake_minimum_required(VERSION 4.3)

project(engine_v29090909 LANGUAGES C CXX)

find_package(Vulkan REQUIRED COMPONENTS volk shaderc_combined)
option(BUILD_SHARED_LIBS "Build GLFW and Assimp as shared libraries" OFF)

add_subdirectory(external/glfw)
add_subdirectory(external/spdlog)

if(APPLE)
    message(STATUS "Configuring for macOS")

    enable_language(Swift)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")

    find_library(METAL_FRAMEWORK Metal REQUIRED)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
    find_library(APPKIT_FRAMEWORK AppKit REQUIRED)
endif()

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
        src/core/Config.hpp
        src/graphics/metal/MetalRenderer.hpp
        src/core/Camera.hpp
        src/platform/WindowMacOS.hpp
        src/platform/IWindow.hpp
        src/platform/WindowGLFW.hpp
        src/core/EngineHeart.hpp
        src/graphics/IRenderer.hpp
        src/graphics/opengl/RendererOpenGL.hpp
        src/graphics/opengl/GlModel.hpp
        src/graphics/opengl/GlMesh.cpp
        src/graphics/opengl/RendererOpenGL.cpp
        src/graphics/IModel.cpp
        src/core/EngineHeart.cpp
        src/core/Config.cpp
        src/core/Camera.cpp
        src/platform/WindowGLFW.cpp
        external/glad/gl.c
        src/graphics/Renderer.cpp
        src/graphics/vulkan/RendererVulkan.hpp
        src/graphics/vulkan/RendererVulkan.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8 /bigobj)
    target_sources(${PROJECT_NAME} PRIVATE
            src/platform/bootstrap/win/windows_main.cpp
            src/platform/bootstrap/win/windows_main.hpp
    )
    target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
    target_link_options(${PROJECT_NAME} PRIVATE "/ENTRY:wWinMainCRTStartup")
#    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
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
            src/engine/platform/WindowMetal.mm
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_EXPOSE_NATIVE_COCOA APP_HAS_SWIFT)
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
    Vulkan::Vulkan
    Vulkan::volk
    Vulkan::shaderc_combined
)

if(WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLL dependencies"
    )
endif()
