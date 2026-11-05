#include "Camera.hpp"
#include "Config.hpp"
#include "../platform/WindowGLFW.hpp"
#include "../graphics/IRenderer.hpp"
#include "EngineHeart.hpp"
#include <spdlog/spdlog.h>

namespace Luna {
    namespace {
        class Penger {
        public:
            IModel* model;

            explicit Penger(Configuration *config) :
            model(IModel::Create(config, "assets/penger-obj-main/penger/penger.obj", false))
            {}

            ~Penger() {}
        };

        Penger *penger = nullptr;
    }

    void EngineHeart::Loop() const {
        glm::mat4 model(1.0f);
        // oh boy he high up there
        model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));

        const auto shader = renderer->shaders[0];
        shader->use();
        shader->setMat4("model", model);
        shader->setMat4("view", renderer->camera->view);
        shader->setMat4("projection", renderer->camera->projection);

        // enable back-face culling for his render
        renderer->setCulling(true);
        penger->model->Draw(*shader);
    }

    EngineHeart::EngineHeart() {
        config = std::make_shared<Configuration>();
        config->Load();

        window = std::make_unique<WindowGLFW>(config, this);
        spdlog::info("Window initialized.");

        renderer = StartBackend(config.get());
        renderer->init(window.get(), config.get());
        spdlog::info("Renderer initialized.");

        auto *shader = IShader::Create(config.get(), "shaders/triangle.vsh", "shaders/triangle.fsh");
        shader->name = "Triangles";
        renderer->shaders.push_back(shader);

        // Initialize objects
        penger = new Penger(config.get());
    }

    int EngineMain(int argc, char **argv) {
        // TODO: argv parsing into a holder data struct
        auto *app = new EngineHeart();
        const auto &window = app->window;
        const auto &renderer = app->renderer;

        // auto *floor = new OpenGL::Floor();
        // floor->Setup();

        window->Show();
        auto last_time = window->GetTime();
        while (!window->ShouldClose()) {
            app->currentTime = window->GetTime();
            renderer->deltaTime = static_cast<float>(app->currentTime - last_time);
            last_time = app->currentTime;
            renderer->camera->Update(*window, renderer->deltaTime);

            renderer->beginFrame();
            renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});

            app->Loop();

            // disable back-face culling for the floor render
            // renderer->setCulling(false);
            // floor->Render(*renderer->camera);

            window->SwapBuffers();
            window->PollEvents();
            renderer->endFrame();
        }

        delete app;
        spdlog::info("Exited successfully!");
        return 0;
    }
}
