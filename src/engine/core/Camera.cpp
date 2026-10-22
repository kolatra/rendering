#include <engine/core/Camera.hpp>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Luna {
    EngineCamera::EngineCamera(Configuration *config) {
        constexpr float fov = glm::radians(45.0f);
        const auto W = static_cast<float>(config->width);
        const auto H = static_cast<float>(config->height);
        projection = glm::perspective(fov, W / H, 0.1f, 100.0f);

        position = glm::vec3(0.0f, 0.0f, 5.0f);
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        view = glm::mat4(1.0f);
    }

    void EngineCamera::Update() {
        view = glm::lookAt(position, position + front, up);
    }
}
