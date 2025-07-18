#pragma once
#include <memory>

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

} // namespace Chained