#pragma once

#include <glm/glm.hpp>

namespace Luna {
    struct Configuration;
    class IWindow;

    struct EngineCamera {
        glm::mat4 projection {};

        glm::vec3 position {};
        glm::vec3 front {};
        glm::vec3 up {};
        glm::mat4 view {};

        explicit EngineCamera(const Configuration *config);
        void Update(IWindow &window, float delta);
    };
}
