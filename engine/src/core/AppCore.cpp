#include "spdlog/spdlog.h"
#include "platform/IWindow.hpp"
#include "graphics/IRenderer.hpp"
#include "core/AppCore.hpp"
#include "core/Camera.hpp"
#include "core/Config.hpp"

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

        app->renderer->camera = std::make_unique<EngineCamera>(*app->config);
        app->holdingMouse = true;
        spdlog::trace("renderer initialized");

        Shader::Create(*app, "triangle");
        app->floor = Floor::Create(*app->config);

        return app;
    }

    static std::string PosToString(glm::vec3 pos) {
        return std::format("x: {} y: {} z: {}", pos.x, pos.y, pos.z);
    }

    int AppCore::Run() {
        auto last_time = window->GetTime();
        while (!window->ShouldClose()) {
            currentTime = window->GetTime();
            renderer->deltaTime = static_cast<float>(currentTime - last_time);
            last_time = currentTime;
            window->SetTitle(PosToString(renderer->camera->position));
            for (const auto &object : renderer->objects) {
                object->Tick();
            }

            renderer->camera->Update(*window, renderer->deltaTime);

            {
                renderer->beginFrame();

                for (const auto &object : renderer->objects) {
                    object->Draw(renderer);
                }

                if (config->showFloor) {
                    renderer->setCulling(false);
                    floor->Render(*renderer->camera);
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
        auto &camera = this->renderer->camera;

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
