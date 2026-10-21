#include <filesystem>
#include <engine/core/Application.hpp>

#include <tomlplusplus/toml.hpp>
#include <spdlog/spdlog.h>

namespace Luna {
    bool Configuration::Load() {
        toml::parse_result config;
        try {
             config = toml::parse_file("config.toml");
        } catch (std::filesystem::filesystem_error& _) {
            spdlog::warn("No config file found. Using defaults.");
            return false;
        }

        if (const std::string api = config["general"]["backend"].value_or(""); api != "") {
            this->rendering_api = be_from_string(api);
        }

        if (const std::string window_name = config["general"]["window_name"].value_or(""); window_name != "") {
            this->name = window_name.c_str();
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
