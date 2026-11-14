#pragma once
#include <memory>
#include <cstdint>
#include <glm/glm.hpp>
#include <graphics/Renderer.hpp>

namespace Luna {
    // a scene is a collection of objects, lights, cameras, terrain, etc.
    // it's basically the holder of everything that's currently on screen.
    // could be a loading screen while everything else happens in the background, then it switches to the main scene

    class GameObject {
    public:
        uint32_t renderPriority = 0;
        glm::mat4 position;
        std::shared_ptr<Model> model;
        std::string modelPath;
        bool culling = false;

        virtual ~GameObject() { model.reset(); }
        virtual void Tick() = 0;
        virtual void Draw(const std::shared_ptr<Renderer> &renderer) const;

        uint32_t GetPriority() const { return renderPriority; }
    };

    class Scene {
    public:
        Scene(std::shared_ptr<Renderer> renderer);
        ~Scene();

        // Holds a pointer to the renderer handling it
        std::shared_ptr<Renderer> renderer;
        std::vector<Shader*> shaders;
        std::unique_ptr<EngineCamera> camera;
        std::vector<std::shared_ptr<GameObject>> objects;

        void Update(float dt);
        void Render(float dt);

        template <typename O>
        void RegisterObject(const std::shared_ptr<Configuration> &config) {
            std::shared_ptr<GameObject> object = std::make_shared<O>();
            object->model = Model::Create(config, object->modelPath);
            objects.push_back(object);
        }
    };
}
