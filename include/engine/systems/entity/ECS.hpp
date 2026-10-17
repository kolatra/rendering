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

    struct EntityComponentSystem {
        std::unique_ptr<std::map<Entity, std::vector<EntityComponent>>> entities;
        void AddComponent(Entity &entity, EntityComponent component);
    };
}
