#include <filesystem>

#include "core/Config.hpp"

#include <iostream>

#include "Utilities.hpp"

using namespace std::literals;
namespace fs = std::filesystem;

namespace Luna {
Configuration::Configuration(bool storeDefaults) {
    if (!Load()) {
        fs::path file = fs::current_path() / "config.toml";
        fmt::println("No config file found at {} - Defaults will be used.", file.string());
        if (storeDefaults) {
            Save(file);
            fmt::println("Saved config to {}", file.string());
        } else {
            fmt::println("Start the program with --save-config to save the defaults to disk.");
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

bool Configuration::Save(std::filesystem::path &path) const {
    auto cwd = std::filesystem::current_path();
    toml::table configTable = this->to_toml();
    toml::table root {
        {"general", std::move(configTable)}
    };

    fmt::println("--- Serialized ---");
    std::cout << root << std::endl;

    std::ofstream outFile(path);
    if (outFile.is_open()) {
        outFile << root << std::endl;
        return true;
    } else {
        fmt::println("Could not open config file for writing at {}", path.string());
        return false;
    }
}

toml::table Configuration::to_toml() const {
    return toml::table {
        {"graphics", be_to_string(this->rendering_api)},
        {"windowName", this->name},
        {"width", this->startingWidth},
        {"height", this->startingHeight},
        {"alwaysLog", this->alwaysLog}
    };
}
}
