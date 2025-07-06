#ifdef CH_EDITOR
#pragma once
#include <imgui.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "../headers/types.h"


namespace Chained {
    class EditorState;
    class SpriteAtlas;
    class Camera;
    class Engine;
    struct SceneObject;
    struct AtlasFrame;
    
    // Forward declaration for AssetEntry
    struct AssetEntry;

    class EditorUI {
    public:
        explicit EditorUI(EditorState* editorState);
        ~EditorUI() = default;

        void render();
        void handleInput();

    private:
        EditorState* m_editorState;
        
        // UI state
        static char sceneNameBuffer[128];
        
        // UI rendering functions
        void renderMainDockSpace();
        void renderAssetPalette();
        void renderEditorControls();
        void renderControlsHelp();
        void renderSceneSaveLoad();
        void renderObjectProperties();
        
        // Physics Editor UI methods
        void renderPhysicsEditor();
        void renderPhysicsBodyProperties(PhysicsBody& physics);
        void renderPhysicsMaterialProperties(PhysicsMaterial& material);
        void renderGameObjectProperties(GameObjectProperties& gameProps);
        void renderPhysicsPreview();
        void renderPhysicsMaterialsLibrary();
        
        // Helper methods
        const char* getBodyTypeString(BodyType type);
        const char* getShapeTypeString(ShapeType type);
        void createPhysicsMaterial(const std::string& name);
        void savePhysicsMaterial(const PhysicsMaterial& material);
        void loadPhysicsMaterial(const std::string& name);

        // Input handling
        void handleMouseInput();
        void handleKeyboardInput();
        void handleObjectPlacement();
        
        // Helper functions
        bool isMouseOverUI() const;
        glm::vec2 getMouseWorldPosition() const;
        bool isObjectUnderMouse(const SceneObject& obj, const glm::vec2& mouseWorldPos) const;
        
        // Physics editor state
        std::vector<PhysicsMaterial> physicsMaterials;
        bool showPhysicsPreview = false;
        bool showMaterialsLibrary = false;
        int selectedMaterialIndex = 0;
    };
}

#endif // CH_EDITOR 