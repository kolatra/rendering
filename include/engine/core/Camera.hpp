#pragma once

#include <glm/glm.hpp>

#include "Config.hpp"

namespace Luna {
    struct EngineCamera {
        glm::mat4 projection {};

        glm::vec3 position {};
        glm::vec3 front {};
        glm::vec3 up {};
        glm::mat4 view {};

        EngineCamera(Configuration *config);
    };
}
