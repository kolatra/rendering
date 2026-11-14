#include "spdlog/spdlog.h"
#include "platform/IWindow.hpp"
#include "graphics/Renderer.hpp"
#include "core/AppCore.hpp"
#include "scene/Scene.hpp"

#include "LunaEngine.hpp"

namespace Luna {
    std::unique_ptr<AppCore> Initialize(int argc, char **argv) {
        bool storeDefaultConfig = false;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--save-config") == 0) {
                storeDefaultConfig = true;
            }
        }
        auto config = std::make_shared<Configuration>(storeDefaultConfig);

        spdlog::trace("creating app core");
        auto app = std::make_unique<AppCore>();
        app->config = std::move(config);

        spdlog::trace("creating window");
        app->window = CreateWindow(app);
        app->window->Show();

        spdlog::trace("initializing rendering");
        app->renderer = StartBackend(*app->config);
        app->renderer->init(*app->window);
        spdlog::trace("renderer initialized");

        return app;
    }

    static std::string PosToString(glm::vec3 pos) {
        return std::format("x: {} y: {} z: {}", pos.x, pos.y, pos.z);
    }

    int AppCore::Run() {
        if (!renderer->currentScene) {
            spdlog::error("Current scene not set! Scenes were not loaded correctly!");
            throw std::runtime_error("Scene not loaded correctly!");
        }
        renderer->currentScene->camera = std::make_unique<EngineCamera>(*config, window);

        // This shader can probably be moved to the scene itself, but it's our default for any
        // triangle based renders
        Shader::Create(*this, renderer->currentScene, "triangle");
        floor = Floor::Create(*this->config);
        holdingMouse = true;

        auto last_time = window->GetTime();
        while (!window->ShouldClose()) {
            currentTime = window->GetTime();
            renderer->deltaTime = static_cast<float>(currentTime - last_time);
            last_time = currentTime;

            renderer->currentScene->Update(renderer->deltaTime);
            window->SetTitle(PosToString(renderer->currentScene->camera->position));

            {
                renderer->beginFrame();

                renderer->currentScene->Render(renderer->deltaTime);

                if (config->showFloor) {
                    renderer->setCulling(false);
                    floor->Render(*renderer->currentScene->camera);
                }

                renderer->endFrame();
            }

            window->SwapBuffers();
            window->PollEvents();
        }
        return 0;
    }

    void AppCore::UpdateMouse(double x, double y) {
        if (!holdingMouse) return;
        auto &camera = this->renderer->currentScene->camera;

        if (camera->firstMouse) {
            this->mouseX = x;
            this->mouseY = y;
            camera->firstMouse = false;
        }

        float xoffset = x - this->mouseX;
        float yoffset = this->mouseY - y;
        this->mouseX = x;
        this->mouseY = y;

        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        camera->yaw += xoffset;
        camera->pitch += yoffset;

        if (camera->pitch > 89.0f)
            camera->pitch = 89.0f;
        if (camera->pitch < -89.0f)
            camera->pitch = -89.0f;

        if (camera->yaw > 360.f)
            camera->yaw = 0.f;
        if (camera->yaw < -360.f)
            camera->yaw = 0.f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
        direction.y = sin(glm::radians(camera->pitch));
        direction.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
        camera->front = glm::normalize(direction);
    }
}
