#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace Luna {
    struct Configuration;
    class IWindow;

    struct EngineCamera {
        std::shared_ptr<IWindow> window;

        glm::mat4 projection {};

        glm::vec3 position {};
        glm::vec3 front {};
        glm::vec3 up {};
        glm::mat4 view {};

        float pitch = 0.f;
        float yaw = -90.f;
        bool firstMouse = true;

        explicit EngineCamera(Configuration &config, std::shared_ptr<IWindow> window);
        void Update(float delta);
    };
}
