#include "core/Camera.hpp"
#include "core/Config.hpp"
#include "platform/IWindow.hpp"
#include "graphics/IRenderer.hpp"
#include "Penguin.h"
#include "core/AppCore.hpp"
#include <spdlog/spdlog.h>

// ReSharper disable once CppUseInternalLinkage
int EngineMain(int argc, char **argv) {
    // TODO: argv parsing into a holder data struct
    auto [app, config, window, renderer] = Luna::Initialize();

    // create our first game object, engine cleans it up
    const auto penger = new Penger(config);
    app->RegisterObject(penger);

    // ENGINE MAIN LOOP START
    window->Show();
    auto last_time = window->GetTime();
    while (!window->ShouldClose()) {
        app->currentTime = window->GetTime();
        renderer->deltaTime = static_cast<float>(app->currentTime - last_time);
        last_time = app->currentTime;
        window->SetTitle(std::format("deltaTime = {}", renderer->deltaTime));

        renderer->camera->Update(*window, renderer->deltaTime);

        {
            renderer->beginFrame();

            for (const auto object : renderer->objects) {
                object->Draw(renderer);
            }

            renderer->setCulling(false);
            app->floor->Render(*renderer->camera);

            renderer->endFrame();
        }

        window->SwapBuffers();
        window->PollEvents();
    }

    app.release();
    spdlog::info("Exited successfully!");
    // ENGINE MAIN LOOP END
    return 0;
}
