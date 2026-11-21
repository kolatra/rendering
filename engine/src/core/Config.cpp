#include <filesystem>
#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "core/Config.hpp"

using namespace std::literals;
namespace fs = std::filesystem;

namespace Luna {

Configuration::Configuration(bool storeDefaults) {
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%^%l%$] %v");

    if (!Load()) {
        fs::path file = fs::current_path() / "config.toml";
        spdlog::warn("No config file found at {} - Defaults will be used.", file.string());
        if (storeDefaults) {
            spdlog::info("Saving config to {}", file.string());
            Save(file);
        } else {
            spdlog::info("Start the program with --save-config to save the defaults to disk.");
        }
    }
}

bool Configuration::Load() {
    try {
         config = toml::parse_file("config.toml");
    } catch (toml::v3::ex::parse_error& _) {
        return false;
    }

    if (const std::string api = config["general"]["graphics"].value_or(""); api != "") {
        this->rendering_api = be_from_string(api);
    }

    if (const std::string_view window_name = config["general"]["windowName"].value_or(""sv); window_name != "") {
        this->name = window_name.data();
    }

    if (const uint32_t w = config["general"]["width"].value_or(0); w != 0) {
        this->startingWidth = w;
    }

    if (const uint32_t h = config["general"]["height"].value_or(0); h != 0) {
        this->startingHeight = h;
    }

    return true;
}


void Configuration::Print() {
    auto currentLevel = spdlog::get_level();
    if (currentLevel > spdlog::level::debug) {
        spdlog::set_level(spdlog::level::debug);
    }

    spdlog::debug("CFG: Graphics API -> {}", this->rendering_api);
    spdlog::debug("CFG: Window name -> {}", this->name);
    spdlog::debug("CFG: Width -> {}", this->startingWidth);
    spdlog::debug("CFG: Height -> {}", this->startingHeight);

    if (spdlog::get_level() != currentLevel) {
        spdlog::set_level(currentLevel);
    }
}

bool Configuration::Save(std::filesystem::path &path) const {
    auto cwd = std::filesystem::current_path();
    toml::table configTable = this->to_toml();
    toml::table root {
        {"general", std::move(configTable)}
    };

    spdlog::debug("--- Serialized ---");
    std::cout << root << std::endl;

    std::ofstream outFile(path);
    if (outFile.is_open()) {
        outFile << root << std::endl;
        return true;
    } else {
        spdlog::error("Could not open config file for writing at {}", path.string());
        return false;
    }
}

toml::table Configuration::to_toml() const {
    return toml::table {
        {"graphics", this->rendering_api},
        {"windowName", this->name},
        {"width", this->startingWidth},
        {"height", this->startingHeight},
    };
}
}
