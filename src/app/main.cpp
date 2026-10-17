#include "../../include/engine/core/Application.hpp"
#include "engine/graphics/Model.hpp"
#include "engine/graphics/opengl/RendererOpenGL.hpp"

#include "../engine/objects/Penger.hpp"

using namespace Luna;

void Application::Run() const {
    Penger penger;
    const auto shader = renderer->shaders[0];

    window->Show();
    while (!window->ShouldClose()) {
        renderer->beginFrame();
        renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});

        penger.Draw(*shader);

        window->SwapBuffers();
        window->PollEvents();
        renderer->endFrame();
    }
}

int main() {
    const auto *app = CreateApp(1280, 720);
    app->Run();

    return 0;
}
