#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>
namespace Chained {

	using Texture2DPtr = std::shared_ptr<class Texture2D>;
	using ShaderPtr = std::shared_ptr<class Shader>;
	using SpriteRendererPtr = std::shared_ptr<class SpriteRenderer>;

	using GameObjectPtr = std::shared_ptr<class GameObject>;
	using PlayerPtr = std::shared_ptr<class Player>;
	using EnemyPtr = std::shared_ptr<class Enemy>;
	using BulletPtr = std::shared_ptr<class Bullet>;
	using RoomPtr = std::shared_ptr<class Room>;
	using DungeonPtr = std::shared_ptr<class Dungeon>;

	using Texture2DPtr = std::shared_ptr<Texture2D>;

	using InputManagerPtr = std::shared_ptr<class InputManager>;
	using AudioManagerPtr = std::shared_ptr<class AudioManager>;


	struct PhysicsMaterial {
		float friction = 0.5f;
		float bounciness = 0.0f;
		float density = 1.0f;
		std::string name = "Default";
	};

	enum class BodyType { Static, Dynamic, Kinematic };
	enum class ShapeType { Box, Circle, Polygon };

	struct PhysicsBody {
		bool enabled = false;
		BodyType bodyType = BodyType::Dynamic;
		ShapeType shapeType = ShapeType::Box;
		float mass = 1.0f;
		float gravityScale = 1.0f;
		float linearDamping = 0.0f;
		float angularDamping = 0.0f;
		bool fixedRotation = false;
		bool isSensor = false;
		PhysicsMaterial material;
		glm::vec2 size = { 1.0f, 1.0f };
		float radius = 0.5f;
	};


	struct SceneObject {
		std::string name;
		glm::vec2 position{};
		float rotation = 0;
		glm::vec2 scale{ 1, 1 };
		int assetId = 0;
		PhysicsBody physics;
	};
	

} // namespace Chained