#include <engine/core/Camera.hpp>
#include "glm/ext/matrix_clip_space.hpp"

namespace Luna {
    EngineCamera::EngineCamera(Configuration *config) {
        constexpr float fov = glm::radians(45.0f);
        const auto W = static_cast<float>(config->width);
        const auto H = static_cast<float>(config->height);
        projection = glm::perspective(fov, W / H, 0.1f, 100.0f);

        position = glm::vec3(0.0f);
        front = glm::vec3(0.0);
        up = glm::vec3(0.0);
        view = glm::mat4(1.0);
    }
}
