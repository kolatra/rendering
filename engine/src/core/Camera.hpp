#pragma once

#include <glm/glm.hpp>

namespace Luna {
    class AppCore;
    struct Configuration;
    struct Window;

    struct EngineCamera {
        Window* window;

        glm::mat4 projection {};

        glm::vec3 position {};
        glm::vec3 front {};
        glm::vec3 up {};
        glm::mat4 view {};

        float pitch = 0.f;
        float yaw = -90.f;
        bool firstMouse = true;

        explicit EngineCamera(Configuration &config, Window* window);
        void Update(float delta, bool controllable);
        void UpdateMouse(AppCore *app, float x, float y);
    };
}
