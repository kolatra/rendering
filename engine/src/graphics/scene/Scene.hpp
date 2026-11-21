#pragma once
#include <memory>
#include <cstdint>
#include <typeinfo>

#include <glm/glm.hpp>
#include <fmt/core.h>

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
        virtual void PreRender() = 0;
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
        void Render();

        template <typename O>
        void RegisterObject() {
            std::unique_ptr<GameObject> object = std::make_unique<O>();
            fmt::println("Registered GameObject type: {}", typeid(O).name());

            if (object->modelPath != "") {
                object->model = Model::Create(object->modelPath);
                object->shader = shaders.at(0).get();
            }
            objects.push_back(std::move(object));
        }
    };

    namespace SceneManager {
        void Startup();
        void Shutdown();
        void Update(float dt);
        void Render();

        std::vector<std::unique_ptr<Scene>> &Scenes();
        EngineCamera &Camera();

        Scene& CurrentScene();
        Scene& CreateScene(AppCore &app, bool controls);
        bool SelectScene(int i);
    }
}
