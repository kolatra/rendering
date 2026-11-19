project(EngineProject LANGUAGES C CXX)

find_package(Vulkan REQUIRED COMPONENTS volk shaderc_combined)
find_package(Freetype REQUIRED)

add_subdirectory(external/glfw)
add_subdirectory(external/spdlog)
add_subdirectory(external/glm)

if(APPLE)
    message(STATUS "Configuring for macOS")

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
        src/graphics/impl/opengl/GlMesh.cpp
        src/graphics/impl/opengl/RendererOpenGL.cpp
        src/graphics/IModel.cpp
        src/core/Config.cpp
        src/core/Camera.cpp
        src/platform/WindowGLFW.cpp
        external/glad/gl.c
        src/graphics/Graphics.cpp
        src/graphics/impl/vulkan/RendererVulkan.cpp
        src/platform/Platform.cpp
        src/core/AppCore.cpp
        src/graphics/impl/opengl/GlFloor.cpp
        src/graphics/scene/Scene.cpp
        src/graphics/TextRendering.cpp
        src/graphics/impl/opengl/TextOpenGL.cpp
        src/util/FileHelper.cpp
        src/managers/AssetManager.hpp
        src/managers/AssetManager.cpp
        src/graphics/types/Shader.hpp
        src/graphics/impl/SpirVCompiler.cpp
        src/graphics/impl/SpirVCompiler.hpp
        src/graphics/scene/SceneManager.cpp
        src/util/Strings.cpp
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8 /bigobj)
    target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
    target_link_options(${PROJECT_NAME} PRIVATE "/ENTRY:wWinMainCRTStartup")
    #    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
        src
        external
        external/spdlog/include
        external/glm
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

    target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_EXPOSE_NATIVE_COCOA APP_HAS_SWIFT)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:OBJCXX>:-fobjc-arc>)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        glm::glm
        assimp::assimp
        Vulkan::Vulkan
        Vulkan::volk
        Vulkan::shaderc_combined
        Freetype::Freetype
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
