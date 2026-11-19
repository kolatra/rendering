#pragma once
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include "graphics/Renderer.hpp"

namespace Luna::Vulkan {
    class Implementation : public Renderer {
    public:
        explicit Implementation() {}
        ~Implementation() override;
        void init(Window &window) override;
        void beginFrame() override;
        void endFrame() override ;
        void clear(const glm::vec4& colour) override;
        void setCulling(bool flag) override;
    };

    class VShader : public Shader {
    public:
        explicit VShader();
        ~VShader() override;

        void use() override;
        void setBool(const std::string &name, bool value) override;
        void setInt(const std::string &name, int value) override;
        void setFloat(const std::string &name, float value) override;
        void setMat4(const std::string &name, const glm::mat4 &value) override;

        VkShaderModule vertexModule = nullptr;
        VkShaderModule fragmentModule = nullptr;

        static VkShaderModule createModule(const std::string &filename, shaderc_shader_kind kind);
    };
}
