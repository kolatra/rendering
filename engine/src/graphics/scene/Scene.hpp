#pragma once
#include <memory>
#include <typeinfo>

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include "graphics/Renderer.hpp"

namespace Luna {
    class Node {
    public:
        std::weak_ptr<Node> parent;
        std::vector<std::shared_ptr<Node>> children;

        glm::mat4 transform = glm::mat4(1.0f);
        std::unique_ptr<Model> model = nullptr;
        Shader* shader = nullptr;
        std::string modelPath = "";
        bool culling = false;

        virtual ~Node() { model.reset(); }
        virtual void Tick() {}
        virtual void PreRender() {}
        virtual void Draw(Renderer &renderer) const;
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
        std::vector<std::unique_ptr<Node>> objects;

        void Update(float dt);
        void Render();

        template <typename O>
        void RegisterObject() {
            std::unique_ptr<Node> object = std::make_unique<O>();
            spdlog::debug("Registered GameObject type: {}", typeid(O).name());

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
