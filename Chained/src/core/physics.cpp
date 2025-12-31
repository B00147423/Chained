#include "../headers/physics.h"  
#include <iostream>

namespace Chained {

    PhysicsSystem::PhysicsSystem(const b2Vec2& gravity) {
        world = new b2World(gravity);
    }

    PhysicsSystem::~PhysicsSystem() {
        clear();
        delete world;
    }

    void PhysicsSystem::addObjects(std::vector<std::unique_ptr<SceneObject>>& objects) {
        for (auto& obj : objects) {
            if (!obj->physics.enabled) continue;

            b2BodyDef bodyDef;

            // Robust enum mapping for body type
            b2BodyType box2dType;
            switch (obj->physics.bodyType) {
            case BodyType::Dynamic:
                box2dType = b2_dynamicBody;
                break;
            case BodyType::Kinematic:
                box2dType = b2_kinematicBody;
                break;
            case BodyType::Static:
            default:
                box2dType = b2_staticBody;
                break;
            }
            bodyDef.type = box2dType;
            // *** Convert position from pixels to meters ***
            bodyDef.position.Set(obj->position.x / PHYSICS_SCALE, obj->position.y / PHYSICS_SCALE);

            bodyDef.angle = obj->rotation;
            bodyDef.fixedRotation = obj->physics.fixedRotation;
            bodyDef.linearDamping = obj->physics.linearDamping;
            bodyDef.angularDamping = obj->physics.angularDamping;

            b2Body* body = world->CreateBody(&bodyDef);

            if (obj->physics.shapeType == ShapeType::Box) {
                b2PolygonShape shape;
                // *** Convert size from pixels to meters ***
                shape.SetAsBox((obj->physics.size.x * 0.5f) / PHYSICS_SCALE, (obj->physics.size.y * 0.5f) / PHYSICS_SCALE);
                b2FixtureDef fixtureDef;
                fixtureDef.shape = &shape;
                fixtureDef.density = obj->physics.material.density;
                fixtureDef.friction = obj->physics.material.friction;
                fixtureDef.restitution = obj->physics.material.bounciness;
                fixtureDef.isSensor = obj->physics.isSensor;
                // Debug print
                std::cout << "[DEBUG] Creating Box Fixture for: " << obj->name << " | Density: " << fixtureDef.density << " | Size: (" << obj->physics.size.x << ", " << obj->physics.size.y << ")" << std::endl;
                body->CreateFixture(&fixtureDef);
            }
            else if (obj->physics.shapeType == ShapeType::Circle) {
                b2CircleShape shape;
                shape.m_p.Set(0, 0);
                // *** Convert radius from pixels to meters ***
                shape.m_radius = obj->physics.radius / PHYSICS_SCALE;
                b2FixtureDef fixtureDef;
                fixtureDef.shape = &shape;
                fixtureDef.density = obj->physics.material.density;
                fixtureDef.friction = obj->physics.material.friction;
                fixtureDef.restitution = obj->physics.material.bounciness;
                fixtureDef.isSensor = obj->physics.isSensor;
                // Debug print
                std::cout << "[DEBUG] Creating Circle Fixture for: " << obj->name << " | Density: " << fixtureDef.density << " | Radius: " << obj->physics.radius << std::endl;
                body->CreateFixture(&fixtureDef);
            }
            bodyMap[obj.get()] = body;
        }
    }

    void PhysicsSystem::step(float dt) {
        const int32 velocityIterations = 6;
        const int32 positionIterations = 2;
        world->Step(dt, velocityIterations, positionIterations);
    }

    void PhysicsSystem::syncToObjects(std::vector<std::unique_ptr<SceneObject>>& objects) {
        for (auto& obj : objects) {
            if (!obj->physics.enabled) continue;
            b2Body* body = bodyMap[obj.get()];
            if (!body) continue;
            b2Vec2 pos = body->GetPosition();
            // *** Convert position from meters to pixels ***
            obj->position.x = pos.x * PHYSICS_SCALE;
            obj->position.y = pos.y * PHYSICS_SCALE;
            obj->rotation = body->GetAngle();
        }
    }

    void PhysicsSystem::clear() {
        for (auto it = bodyMap.begin(); it != bodyMap.end(); ++it) {
            world->DestroyBody(it->second);
        }
        bodyMap.clear();
    }

    b2Body* PhysicsSystem::getBodyFor(SceneObject* obj) {
        auto it = bodyMap.find(obj);
        return (it != bodyMap.end()) ? it->second : nullptr;
    }

}