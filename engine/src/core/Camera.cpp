#include "core/Camera.hpp"
#include "core/Config.hpp"
#include "platform/IWindow.hpp"
#include "core/Keys.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace Luna {
    EngineCamera::EngineCamera(Configuration &config, std::shared_ptr<IWindow> window) {
        this->window = window;

        constexpr float fov = glm::radians(45.0f);
        const auto W = static_cast<float>(config.startingWidth);
        const auto H = static_cast<float>(config.startingHeight);
        projection = glm::perspective(fov, W / H, 0.1f, 100.0f);

        position = glm::vec3(0.0f, 1.0f, 5.0f);
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        view = glm::mat4(1.0f);
    }

    void EngineCamera::Update(float delta) {
        auto sens = 2.0f * delta;

        if (window->held_input[KEY_Q])
            window->Close();

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

        // update view
        view = glm::lookAt(position, position + front, up);
    }
}
