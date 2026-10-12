#pragma once

#include <map>
#include <vector>
#include <memory>

namespace Luna::ECS {
    // These represent behaviours of different entities, multiple can be added
    struct EntityComponent {

    };

    struct Entity {
        uint32_t id {};
        std::vector<EntityComponent> components {};
    };

    typedef std::vector<Entity> Entities;

    // The central Entity Component System
    struct System {
        std::unique_ptr<std::map<Entity, std::vector<EntityComponent>>> entities;
        System();
        void AddComponent(Entity &entity, EntityComponent component);
    };
}
