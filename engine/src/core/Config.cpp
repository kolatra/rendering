#include <filesystem>
#include <spdlog/spdlog.h>

#include "core/Config.hpp"

#include <iostream>

using namespace std::literals;
namespace fs = std::filesystem;

namespace Luna {
Configuration::Configuration(bool storeDefaults) {
    spdlog::set_level(spdlog::level::trace);

    if (!Load()) {
        fs::path file = fs::current_path() / "config.toml";
        spdlog::warn("No config file found at {} - Defaults will be used.", file.string());
        if (storeDefaults) {
            Save(file);
            spdlog::info("Saved config to {}", file.string());
        } else {
            spdlog::warn("Start the program with --save-config to save the defaults to disk.");
        }
    }
}

bool Configuration::Load() {
    try {
         config = toml::parse_file("config.toml");
    } catch (toml::v3::ex::parse_error& _) {
        if (this->alwaysLog) {
            this->Print();
        }
        return false;
    }

    if (const std::string api = config["general"]["backend"].value_or(""); api != "") {
        this->rendering_api = be_from_string(api);
    }

    if (const std::string_view window_name = config["general"]["window_name"].value_or(""sv); window_name != "") {
        this->name = window_name.data();
    }

    if (const uint32_t w = config["general"]["width"].value_or(0); w != 0) {
        this->startingWidth = w;
    }

    if (const uint32_t h = config["general"]["height"].value_or(0); h != 0) {
        this->startingHeight = h;
    }

    if (const std::string log_level = config["general"]["log_level"].value_or(""); log_level != "") {
        if (log_level == "debug") {
            spdlog::set_level(spdlog::level::debug);
        } else if (log_level == "info") {
            spdlog::set_level(spdlog::level::info);
        } else if (log_level == "warning") {
            spdlog::set_level(spdlog::level::warn);
        } else if (log_level == "error") {
            spdlog::set_level(spdlog::level::err);
        } else if (log_level == "critical") {
            spdlog::set_level(spdlog::level::critical);
        } else if (log_level == "off") {
            spdlog::set_level(spdlog::level::off);
        }
        std::cout << "spdlog::level::" << log_level << std::endl;
    } else {
        std::cout << "spdlog::level::info (unspecified config option)\n";
        spdlog::set_level(spdlog::level::info);
    }

    if (this->alwaysLog) {
        this->Print();
    }

    return true;
}

void Configuration::Print() {
    auto currentLevel = spdlog::get_level();
    if (currentLevel > spdlog::level::debug) {
        spdlog::set_level(spdlog::level::debug);
    }

    spdlog::debug("CFG: Graphics API -> {}", be_to_string(this->rendering_api));
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

    spdlog::info("--- Serialized ---");
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
        {"graphics", be_to_string(this->rendering_api)},
        {"window_name", this->name},
        {"width", this->startingWidth},
        {"height", this->startingHeight},
        {"showFloor", this->showFloor},
        {"alwaysLog", this->alwaysLog},
        {"log_level", level_to_string(this->logLevel)}
    };
}
}
