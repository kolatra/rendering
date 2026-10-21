#include <filesystem>
#include <spdlog/spdlog.h>
#include <engine/core/Config.hpp>

using namespace std::literals;

namespace Luna {
    bool Configuration::Load() {
        try {
             config = toml::parse_file("config.toml");
        } catch (std::filesystem::filesystem_error& _) {
            spdlog::warn("No config file found. Using defaults.");
            return false;
        }

        if (const std::string api = config["general"]["backend"].value_or(""); api != "") {
            this->rendering_api = be_from_string(api);
        }

        if (const std::string_view window_name = config["general"]["window_name"].value_or(""sv); window_name != "") {
            this->name = window_name.data();
        }

        if (const uint32_t w = config["general"]["width"].value_or(0); w != 0) {
            this->width = w;
        }

        if (const uint32_t h = config["general"]["height"].value_or(0); h != 0) {
            this->height = h;
        }

        return true;
    }
}
