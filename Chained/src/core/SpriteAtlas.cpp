#include "../headers/SpriteAtlas.h"
#include "../headers/resourceManager.h"
#include "../headers/types.h"
#include <fstream>
#include <iostream>

SpriteAtlas::SpriteAtlas(const std::string& jsonFile) {
    std::ifstream file(jsonFile);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open Aseprite JSON: " << jsonFile << "\n";
        std::exit(-1);
    }

    nlohmann::json j;
    file >> j;

    auto meta = j["meta"];
    int atlasW = meta["size"]["w"];
    int atlasH = meta["size"]["h"];
    std::string imageFile = meta["image"];

    m_texture = Chained::ResourceManager::get()->loadTexture(imageFile.c_str(), true, imageFile);

    // Load all frames
    for (auto& [frameName, frameData] : j["frames"].items()) {
        auto frame = frameData["frame"];
        float x = frame["x"];
        float y = frame["y"];
        float w = frame["w"];
        float h = frame["h"];

        glm::vec4 uv = glm::vec4(
            x / float(atlasW),
            1.0f - (y + h) / float(atlasH),  // Flip Y!
            w / float(atlasW),
            h / float(atlasH)
        );

        m_frames[frameName] = { uv, frameData["duration"] };
    }

    // ✅ Load slices
    for (auto& slice : j["meta"]["slices"]) {
        std::string name = slice["name"];
        if (name.empty()) continue; // Skip unnamed slices

        auto bounds = slice["keys"][0]["bounds"];
        float x = bounds["x"];
        float y = bounds["y"];
        float w = bounds["w"];
        float h = bounds["h"];

        glm::vec4 uv = glm::vec4(
            x / float(atlasW),
            (atlasH - y - h) / float(atlasH), // Invert Y coordinate
            w / float(atlasW),
            h / float(atlasH)
        );
    
        m_slices[name] = { uv, 0 };
    }
}

Chained::Texture2DPtr SpriteAtlas::getTexture() const {
    return m_texture;
}

const AtlasFrame& SpriteAtlas::getFrame(const std::string& name) const {
    return m_frames.at(name);
}

const AtlasFrame& SpriteAtlas::getSlice(const std::string& name) const {
    return m_slices.at(name);
}

const std::unordered_map<std::string, AtlasFrame>& SpriteAtlas::getAllFrames() const {
    return m_frames;
}
