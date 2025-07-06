#ifdef CH_EDITOR
#pragma once

#include "GameState.h"
#include "Engine.h"
#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "SpriteAtlas.h"
#include "../headers/Camera.h"
#include "../headers/types.h"


namespace Chained {
    class SpriteRenderer;

    class EditorState : public GameState
    {
    public:
        explicit EditorState(Engine* eng);

        void onEnter() override;
        void onExit() override;
        void update(float dt) override;
        void render() override;
        EngineState result = EngineState::Editor;

    private:


        struct AssetEntry {
            std::string name;
            AtlasFrame frame;
        };
        void drawCameraBounds();
        bool isObjectUnderMouse(const SceneObject& obj, const glm::vec2& mouseWorldPos) const;
        Engine* engine = nullptr;
        std::vector<SceneObject> objects;
        int selectedAsset = 0;
        int selectedObjectIndex = -1;
        bool placementMode = false;
        std::unique_ptr<SpriteRenderer> renderer;
        std::vector<AssetEntry> assetPalette;
        std::shared_ptr<SpriteAtlas> spriteAtlas;
        void saveSceneToJson(const std::string& filename);
        void loadSceneFromJson(const std::string& filename);

        std::unique_ptr<Chained::Camera> camera;
        std::string currentSceneName;
        ShaderPtr m_shader;
    };

} // namespace Chained
#endif