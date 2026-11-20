#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "core/Camera.hpp"
#include "core/Config.hpp"
#include "platform/Window.hpp"
#include "core/Keys.hpp"
#include "core/AppCore.hpp"

namespace Luna {
    EngineCamera::EngineCamera(Configuration &config, Window *window) {
        this->window = window;

        constexpr float fov = glm::radians(45.0f);
        const auto W = static_cast<float>(config.startingWidth);
        const auto H = static_cast<float>(config.startingHeight);
        projection = glm::perspective(fov, W / H, 0.1f, 100.0f);

        position = glm::vec3(0.0f, 0.5f, 2.0f);
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        view = glm::mat4(1.0f);
    }

    void EngineCamera::Update(float delta, bool controllable) {
        if (window->held_input[KEY_Q])
            window->Close();

        if (controllable) {
            auto sens = 2.0f * delta;

            if (window->held_input[KEY_LEFT_SHIFT])
                sens *= 2.5f;

            if (window->held_input[KEY_SPACE])
                position += sens * up;

            if (window->held_input[KEY_LEFT_CONTROL])
                position -= sens * up;

            if (window->held_input[KEY_W])
                position += sens * front;

            if (window->held_input[KEY_S])
                position -= sens * front;

            if (window->held_input[KEY_D])
                position += glm::normalize(glm::cross(front, up)) * sens;

            if (window->held_input[KEY_A])
                position -= glm::normalize(glm::cross(front, up)) * sens;
        }

        // update view
        view = glm::lookAt(position, position + front, up);
    }

    void EngineCamera::UpdateMouse(AppCore *app, float x, float y) {
        if (firstMouse) {
            app->window->mouseX = x;
            app->window->mouseY = y;
            firstMouse = false;
        }

        float xoffset = x - app->window->mouseX;
        float yoffset = app->window->mouseY - y;
        app->window->mouseX = x;
        app->window->mouseY = y;

        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        if (yaw > 360.f)
            yaw = 0.f;
        if (yaw < -360.f)
            yaw = 0.f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);
    }
}
