# C++ Code Structure

## Overview

This folder contains a small C++ rendering-engine prototype named **Luna Rendering Engine**. The code is organized around an `Application` that owns a platform window and a renderer through interfaces. The currently selected implementation is GLFW + OpenGL. Vulkan and Metal appear in the backend enum, but are not implemented in the active code.

## Source layout

```text
src/
├── main.cpp
└── engine/
    ├── Application.hpp / Application.cpp
    ├── platform/
    │   ├── IWindow.hpp
    │   └── glfw/
    │       └── WindowGLFW.hpp / WindowGLFW.cpp
    ├── graphics/
    │   ├── IRenderer.hpp
    │   ├── Model.hpp / Model.cpp
    │   └── opengl/
    │       └── RendererOpenGL.hpp / RendererOpenGL.cpp
    └── objects/
        └── Penger.hpp / Penger.cpp

shaders/
├── triangle.vsh / triangle.fsh
└── text.vsh / text.fsh
```

## Runtime flow

1. `src/main.cpp` constructs an `Application` for a 1280×720 window and calls `run()`.
2. `Application` selects `Backend::OpenGL`, creates a `WindowGLFW`, then creates and initializes a `RendererOpenGL`.
3. `Application::run()` loads the triangle shader pair, creates a `Penger` object (which loads the bundled Penger OBJ model), shows the window, and enters the frame loop.
4. Each frame clears the color and depth buffers, asks `Penger` to draw its model with the shader, swaps the GLFW buffers, and polls window events.

## Main components

### Application — `src/engine/Application.hpp`, `src/engine/Application.cpp`

The top-level coordinator owns `std::unique_ptr<IWindow>` and `std::unique_ptr<IRenderer>`. It chooses the rendering backend, wires the window and renderer together, and controls the frame loop. The backend is currently hard-coded to OpenGL in the constructor; `setBackend()` exists but is not used to configure construction.

### Platform window — `src/engine/platform/`

`IWindow.hpp` defines the window contract and the `Backend` enum (`None`, `OpenGL`, `Vulkan`, `Metal`). `WindowGLFW` implements the contract using GLFW: initialization, context creation, GL function loading through GLAD, callbacks, visibility, event polling, buffer swapping, and close-state checks. Its Vulkan surface method is only a stub, and its Vulkan setup path throws as unimplemented. The key, cursor, and mouse callbacks are currently empty; resize updates the OpenGL viewport.

### Rendering interfaces and OpenGL implementation — `src/engine/graphics/`

`IRenderer.hpp` defines shader, vertex, texture, mesh, and renderer abstractions. `IRenderer` exposes initialization, frame boundaries, mesh drawing, and clearing. `IShader` exposes basic uniform setters. `GlMesh` stores vertex/index/texture data and owns OpenGL buffer handles.

`RendererOpenGL` implements renderer initialization and buffer clearing. `GLShader` reads vertex and fragment shader files, compiles them, links a program, and exposes `use()` and uniform setter methods. The uniform setters and mesh-drawing method are currently mostly placeholders. `GlMesh` uploads vertex/index buffers and configures position, normal, and texture-coordinate attributes; its `Draw()` binds textures and issues an indexed triangle draw.

### Model loading — `src/engine/graphics/Model.hpp`, `src/engine/graphics/Model.cpp`

`Model` uses Assimp to read a model file, recursively visits the scene nodes, converts each Assimp mesh into engine `Vertex`/index/texture data, and creates a `GlMesh`. Material texture paths are cached per model and loaded by `TextureFromFile()` in the OpenGL renderer implementation. Although `Model` is presented as a general graphics type, its mesh storage and texture upload path are currently OpenGL-specific.

### Scene object — `src/engine/objects/Penger.hpp`, `src/engine/objects/Penger.cpp`

`Penger` is a thin example object. It constructs a `Model` from `penger-obj-main/penger/penger.obj` and forwards `Draw()` to that model. The model data itself is under the bundled `penger-obj-main/` directory.

## Shaders

- `shaders/triangle.vsh` and `triangle.fsh` are the pair loaded by the application loop. They declare model/view/projection and texture-related inputs, though the current application does not set those uniforms in the shown flow.
- `shaders/text.vsh` and `text.fsh` form a separate text-rendering shader pair. No text-rendering C++ subsystem appears in `src/`.

## Build and bundled dependencies

The root `CMakeLists.txt` builds one executable from `src/main.cpp` and the engine sources. It requests C++26, finds Vulkan components, and links GLFW, FreeType, Assimp, and Vulkan libraries using configured local paths. The source tree also contains bundled headers under `include/`, including GLAD, GLM, stb_image, tinyobjloader, and CLI11. `include/glad/gl.c` is compiled into the executable. The existence of a library in the build configuration does not imply a corresponding implemented engine subsystem.

## Current boundaries and caveats

- The architecture has interface seams for windows and renderers, but mesh and shader types expose or implement OpenGL details, so the graphics layer is not yet backend-neutral.
- `Backend::Vulkan` and `Backend::Metal` are placeholders; the application only creates `RendererOpenGL`.
- `beginFrame()` and `endFrame()` are empty in `RendererOpenGL`; the current loop clears and draws directly.
- The renderer's `drawMesh()` currently only casts the shader type, and the shader uniform setters are commented out. Consequently, the model/shader path is only partially connected.
- Shader compilation/link checks are commented out, and the text shaders have no corresponding C++ text renderer in this source tree.
- Build configuration uses machine-specific dependency paths. Consult `CMakeLists.txt` for the exact dependency setup.

## Suggested reading order

1. `src/main.cpp`
2. `src/engine/Application.cpp`
3. `src/engine/platform/IWindow.hpp` and `src/engine/platform/glfw/WindowGLFW.cpp`
4. `src/engine/graphics/IRenderer.hpp` and `src/engine/graphics/opengl/RendererOpenGL.cpp`
5. `src/engine/graphics/Model.cpp`
6. `src/engine/objects/Penger.cpp`
