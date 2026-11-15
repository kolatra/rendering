#pragma once
#include <cstdint>
#include <map>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace Luna::FontRenderer {
    struct Character {
        uint32_t texture_id;
        glm::ivec2 size;
        glm::ivec2 bearing;
        uint32_t advance;
    };

    inline std::map<char, Character> character_map;
    inline void AddCharacter(uint8_t c, Character character) {
        character_map.insert(std::pair<char, Character>(c, character));
    }

    void Init();

    namespace OpenGLText {
        void init(void* face);
        void render(const std::string &text, float x, float y, float scale, glm::vec3 colour);
    }
}
