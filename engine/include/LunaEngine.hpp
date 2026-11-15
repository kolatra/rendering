#pragma once

#include "core/AppCore.hpp"
#include "core/Camera.hpp"
#include "core/Config.hpp"
#include "core/Keys.hpp"
#include "scene/Scene.hpp"
#include "graphics/Text.hpp"

namespace Luna {
    class AppCore;
    struct Configuration;
    class GameObject;

    void RenderText(const std::string &text, float x, float y, float scale, glm::vec3 colour);
}