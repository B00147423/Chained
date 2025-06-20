#pragma
#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct GameObjectData {
	std::string id;
	std::string type;
	std::string texture;
	glm::vec2 position;
	glm::vec2 size;
	float rotation = 0.0f;
	std::map<std::string, std::string> properties;
};


struct GameScene {
	std::string name;
	std::vector<GameObjectData> objects;
};

inline void to_json(json& j, const GameObjectData& obj) {
	j = json{
		{"id", obj.id},
		{"type", obj.type},
		{"texture", obj.texture},
		{"position", {obj.position.x, obj.position.y}},
		{"size", {obj.size.x, obj.size.y}},
		{"rotation", obj.rotation},
		{"properties", obj.properties}
	};
}

inline void from_json(const json& j, GameObjectData& obj) {
	obj.id = j.at("id").get<std::string>();
	obj.type = j.at("type").get<std::string>();
	obj.texture = j.at("texture").get<std::string>();
	obj.rotation = j.at("rotation").get<float>();
	auto pos = j.at("position");
	obj.position = glm::vec2(pos[0], pos[1]);
	auto sz = j.at("size");
	obj.size = glm::vec2(sz[0], sz[1]);
	obj.properties = j.at("properties").get<std::map<std::string, std::string>>();
}


// Serialization for GameScene
inline void to_json(json& j, const GameScene& scene) {
	j = json{
		{"name", scene.name},
		{"objects", scene.objects}
	};
}

inline void from_json(const json& j, GameScene& scene) {
	scene.name = j.at("name").get<std::string>();
	scene.objects = j.at("objects").get<std::vector<GameObjectData>>();
}