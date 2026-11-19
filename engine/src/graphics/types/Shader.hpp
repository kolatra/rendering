#pragma once
#include <string>
#include <memory>

#include <glm/glm.hpp>

namespace Luna {
    class Shader {
    public:
        std::string name = "Unknown";

        Shader() = default;
        [[maybe_unused]] static std::unique_ptr<Shader> Create(const std::string &name);
        virtual ~Shader() = default;
        virtual void use() = 0;
        virtual void setBool(const std::string &name, bool value) = 0;
        virtual void setInt(const std::string &name, int value) = 0;
        virtual void setFloat(const std::string &name, float value) = 0;
        virtual void setMat4(const std::string &name, const glm::mat4 &value) = 0;
    };
}
