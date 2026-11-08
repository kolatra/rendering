#include "Camera.hpp"
#include "Config.hpp"
#include "../platform/IWindow.hpp"
#include "../graphics/IRenderer.hpp"
#include "AppCore.hpp"
#include <spdlog/spdlog.h>

namespace Luna {
int EngineMain(int argc, char **argv) {
    // TODO: argv parsing into a holder data struct
    auto app = Initialize();
    auto [config, window, renderer] = GetHandles(app);

    window->Show();
    auto last_time = window->GetTime();
    while (!window->ShouldClose()) {
        app->currentTime = window->GetTime();
        renderer->deltaTime = static_cast<float>(app->currentTime - last_time);
        last_time = app->currentTime;
        renderer->camera->Update(*window, renderer->deltaTime);

        renderer->beginFrame();
        // disable back-face culling for the floor render
        renderer->setCulling(false);
        app->floor->Render(*renderer->camera);

        window->SwapBuffers();
        window->PollEvents();
        renderer->endFrame();
    }

    app.release();
    spdlog::info("Exited successfully!");
    return 0;
}
}
