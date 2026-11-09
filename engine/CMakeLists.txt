project(EngineProject LANGUAGES C CXX)

find_package(Vulkan REQUIRED COMPONENTS volk shaderc_combined)

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

if(TARGET assimp)
    # it won't build for some reason even though the variables are used
    target_compile_options(assimp PRIVATE
            -Wno-error=unused-variable
            -Wno-error=unused-but-set-variable
            -Wno-unused-but-set-variable
    )
endif()

add_library(${PROJECT_NAME}
        src/graphics/opengl/GlMesh.cpp
        src/graphics/opengl/RendererOpenGL.cpp
        src/graphics/IModel.cpp
        src/core/Config.cpp
        src/core/Camera.cpp
        src/platform/glfw/WindowGLFW.cpp
        external/glad/gl.c
        src/graphics/Graphics.cpp
        src/graphics/vulkan/RendererVulkan.cpp
        src/platform/Platform.cpp
        src/core/AppCore.cpp
        src/graphics/opengl/GlFloor.cpp
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8 /bigobj)
    target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
    target_link_options(${PROJECT_NAME} PRIVATE "/ENTRY:wWinMainCRTStartup")
    #    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
        include
        external
        external/spdlog/include
)

if(APPLE)
    target_include_directories(${PROJECT_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/external/metal-cpp
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${METAL_FRAMEWORK}
            ${QUARTZCORE_FRAMEWORK}
            ${APPKIT_FRAMEWORK}
    )

    add_library(${PROJECT_NAME} PRIVATE
            engine/engine/platform/WindowMetal.mm
            engine/platform/macos/WindowMacOS.hpp
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_EXPOSE_NATIVE_COCOA APP_HAS_SWIFT)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:OBJCXX>:-fobjc-arc>)
endif()

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
