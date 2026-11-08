#include "spdlog/spdlog.h"
#include "../platform/IWindow.hpp"
#include "../graphics/IRenderer.hpp"
#include "AppCore.hpp"
#include "Camera.hpp"
#include "Config.hpp"

namespace Luna {
    std::unique_ptr<AppCore> Initialize() {
        auto app = std::make_unique<AppCore>();
        app->config = std::make_shared<Configuration>();
        app->config->Load();

        app->window = CreateAppWindow(app->config, app.get());
        spdlog::info("Window initialized.");

        app->renderer = StartBackend(app->config.get());
        app->renderer->init(app->window.get(), app->config.get());
        // I think I want to keep the camera optional? Because maybe sometimes it'll just be a render without controls
        app->renderer->camera = std::make_unique<EngineCamera>(app->config.get());
        spdlog::info("Renderer initialized.");

        IShader::Create(app.get(), "triangle");

        app->floor = Floor::Create(app->config.get());

        return app;
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
