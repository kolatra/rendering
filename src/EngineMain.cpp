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
    auto app = Luna::Initialize();
    auto [config, window, renderer] = Luna::GetHandles(app);

    // create our first game object, engine handles lifetime
    Penger penger(config.get());
    app->RegisterObject(&penger);

    window->Show();
    auto last_time = window->GetTime();
    while (!window->ShouldClose()) {
        app->currentTime = window->GetTime();
        renderer->deltaTime = static_cast<float>(app->currentTime - last_time);
        last_time = app->currentTime;
        window->SetTitle(std::format("renderer->deltaTime = {}", renderer->deltaTime));

        renderer->camera->Update(*window, renderer->deltaTime);

        {
            renderer->beginFrame();

            // constexpr struct {
            //     bool operator()(const Luna::GameObject *lhs, const Luna::GameObject *rhs) const {  return lhs->GetPriority() < rhs->GetPriority(); }
            // } Comparator;
            // std::ranges::sort(renderer->objects, Comparator);

            for (const auto object : renderer->objects) {
                object->Draw(renderer);
            }

            // disable back-face culling for the floor render
            renderer->setCulling(false);
            app->floor->Render(*renderer->camera);

            renderer->endFrame();
        }

        window->SwapBuffers();
        window->PollEvents();
    }

    app.release();
    spdlog::info("Exited successfully!");
    return 0;
}
