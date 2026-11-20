#include <freetype/freetype.h>
#include FT_FREETYPE_H
#include "spdlog/spdlog.h"

#include "LunaEngine.hpp"
#include "core/Config.hpp"
#include "graphics/Text.hpp"

namespace Luna {
    void FontRenderer::Init() {
        spdlog::debug("Starting FT initialization");

        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            spdlog::error("Could not init FreeType Library");
            throw std::runtime_error("Could not init FreeType Library");
        }

        FT_Face face;
        if (FT_New_Face(ft, "/usr/share/fonts/jetbrains-mono-fonts/JetBrainsMono-Regular.otf", 0, &face)) {
            spdlog::error("Could not load font");
            throw std::runtime_error("Could not load font");
        }

        FT_Set_Pixel_Sizes(face, 0, 48);

        if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
            spdlog::error("Could not render character");
            throw std::runtime_error("Could not render character");
        }

        spdlog::debug("Loading into OpenGL...");
        OpenGLText::init(face);

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void FontRenderer::Shutdown() {
        if (ACTIVE_BACKEND == Backend::OpenGL) {
            OpenGLText::shutdown();
        }
        character_map.clear();
    }

    void RenderText(const std::string &text, float x, float y, float scale, glm::vec3 colour) {
        if (ACTIVE_BACKEND == Backend::OpenGL) {
            FontRenderer::OpenGLText::render(text, x, y, scale, colour);
        }
    }
}
