
#pragma once
#include <box2d/box2d.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include "types.h" // where you define SceneObject
#include "DebugDraw.h"

namespace Chained {

    class PhysicsSystem {
       
    public:
        static constexpr float PHYSICS_SCALE = 32.0f;

        PhysicsSystem(const b2Vec2& gravity);
        ~PhysicsSystem();

        void addObjects(std::vector<std::unique_ptr<SceneObject>>& objects);
        void step(float dt);
        void syncToObjects(std::vector<std::unique_ptr<SceneObject>>& objects);
        void clear();

        b2Body* getBodyFor(SceneObject* obj);
        
        // Get the Box2D world for debug drawing
        b2World* getWorld() const { return world; }
        
    private:
        b2World* world = nullptr;
        std::unordered_map<SceneObject*, b2Body*> bodyMap;
    };

}
