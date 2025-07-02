#pragma once
#pragma once
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include "texture2d.h"
#include <nlohmann/json.hpp>
#include "../headers/types.h"

namespace Chained {

    struct AtlasFrame {
        glm::vec4 uvRect; // x, y, w, h in UV space
        int duration;
    };

    class SpriteAtlas {
    public:
        SpriteAtlas(const std::string& jsonFile);

        Chained::Texture2DPtr getTexture() const;
        const AtlasFrame& getFrame(const std::string& name) const;
        const AtlasFrame& getSlice(const std::string& name) const;
        const std::unordered_map<std::string, AtlasFrame>& getAllFrames() const;

    private:
        Chained::Texture2DPtr m_texture;
        std::unordered_map<std::string, AtlasFrame> m_frames;
        std::unordered_map<std::string, AtlasFrame> m_slices; // added
    };

} // namespace Chained
