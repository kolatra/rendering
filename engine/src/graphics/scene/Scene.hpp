#pragma once
#include <memory>
#include <cstdint>
#include <typeinfo>

#include <glm/glm.hpp>
#include "spdlog/spdlog.h"

#include "graphics/Renderer.hpp"

namespace Luna {

    class GameObject {
    public:
        uint32_t renderPriority = 0;
        glm::mat4 position = glm::mat4(1.0f);
        std::unique_ptr<Model> model = nullptr;
        Shader* shader = nullptr;
        std::string modelPath = "";
        bool culling = false;

        virtual ~GameObject() { model.reset(); }
        virtual void Tick() = 0;
        virtual void PreRender(float dt) = 0;
        virtual void Draw(Renderer &renderer) const;

        uint32_t GetPriority() const { return renderPriority; }
    };

    class Scene {
    public:
        Scene(AppCore &app, bool controls = true);
        ~Scene();

        bool controlsEnabled;

        Renderer& renderer;
        Configuration& config;

        // Managed by the scene
        std::unique_ptr<EngineCamera> camera;
        std::vector<std::unique_ptr<Shader>> shaders;
        std::vector<std::unique_ptr<GameObject>> objects;

        void Update(float dt);
        void Render(float dt);

        template <typename O>
        void RegisterObject() {
            std::unique_ptr<GameObject> object = std::make_unique<O>();
            spdlog::info("Registered GameObject type: {}", typeid(O).name());

            if (object->modelPath != "") {
                object->model = Model::Create(object->modelPath);
                object->shader = shaders.at(0).get();
            }
            objects.push_back(std::move(object));
        }
    };

    struct SceneManager {
        std::vector<std::unique_ptr<Scene>>& Scenes();
        Scene& CreateScene(AppCore &app, bool controls = true);
        void AddScene(std::unique_ptr<Scene> scene);
        Scene& SelectScene(int index);
    private:
        std::vector<std::unique_ptr<Scene>> scenes;
    };
}
