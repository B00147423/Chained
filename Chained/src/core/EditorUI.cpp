#ifdef CH_EDITOR
#include "../headers/EditorUI.h"
#include "../headers/EditorState.h"
#include "../headers/SpriteAtlas.h"
#include "../headers/types.h"
#include "../Game/uiStates/TestState.h"
#include <memory>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <cstring>

using json = nlohmann::json;
using namespace Chained;

// Static member initialization
char EditorUI::sceneNameBuffer[128] = "";

EditorUI::EditorUI(EditorState* editorState)
    : m_editorState(editorState)
{
}

void EditorUI::render() {
    renderMainDockSpace();
    renderAssetPalette();
    renderEditorControls();
    renderControlsHelp();
    renderSceneSaveLoad();
    renderObjectProperties();
}

void EditorUI::handleInput() {
    handleKeyboardInput();
    handleMouseInput();
}

void EditorUI::renderMainDockSpace() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("MainDockSpace", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground);

    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}

void EditorUI::renderAssetPalette() {
    ImGui::Begin("Assets");
    
    // Asset info
    ImGui::Text("Assets loaded from: assets/textures/sprites.json");
    ImGui::Text("Total assets: %d", (int)m_editorState->getAssetPalette().size());
    ImGui::Separator();
    
    const auto& assetPalette = m_editorState->getAssetPalette();
    auto spriteAtlas = m_editorState->getSpriteAtlas();
    
    for (int i = 0; i < assetPalette.size(); ++i) {
        const auto& asset = assetPalette[i];
        ImTextureID texId = (ImTextureID)(intptr_t)spriteAtlas->getTexture()->m_id;
        glm::vec4 uv = asset.frame.uvRect;
        uv.y = 1.f - uv.y - uv.w;
        ImVec2 uv0(uv.x, uv.y);
        ImVec2 uv1(uv.x + uv.z, uv.y + uv.w);
        ImGui::PushID(i);
        if (ImGui::Selectable(asset.name.c_str(), i == m_editorState->getSelectedAsset())) {
            // Enter placement mode when selecting an asset
            if (m_editorState->getSelectedAsset() == i) {
                // If clicking the same asset, exit placement mode
                m_editorState->setSelectedAsset(-1);
                m_editorState->setPlacementMode(false);
            } else {
                // Select new asset and enter placement mode
                m_editorState->setSelectedAsset(i);
                m_editorState->setPlacementMode(true);
            }
        }
        ImGui::SameLine();
        ImGui::Image(texId, ImVec2(24, 24), uv0, uv1);
        ImGui::PopID();
    }
    ImGui::End();
}

void EditorUI::renderEditorControls() {
    ImGui::Begin("Editor Controls");
    if (ImGui::Button("Test")) {
        m_editorState->saveSceneToJson("scenes/temp_test.json");
        m_editorState->getEngine()->run(std::make_unique<TestState>("scenes/temp_test.json"));
    }
    ImGui::End();
}

void EditorUI::renderControlsHelp() {
    ImGui::Begin("Controls");
    ImGui::Text("Asset Palette: Click asset to enter placement mode");
    ImGui::Text("Placement Mode: Click in scene to place object");
    ImGui::Text("Escape: Exit placement mode / Deselect object");
    ImGui::Text("Right Click + Drag: Pan camera");
    ImGui::Text("Mouse Wheel: Zoom camera");
    ImGui::Text("Delete Key: Delete selected object");
    ImGui::End();
}

void EditorUI::renderSceneSaveLoad() {
    ImGui::Begin("Scene Save/Load");
    
    ImGui::InputText("Scene Name", sceneNameBuffer, IM_ARRAYSIZE(sceneNameBuffer));

    if (ImGui::Button("Save Scene")) {
        std::cout << "[DEBUG] Save Scene button pressed, buffer: '" << sceneNameBuffer << "'\n";
        if (strlen(sceneNameBuffer) > 0) {
            m_editorState->setCurrentSceneName(sceneNameBuffer);
            std::string path = std::string("scenes/") + m_editorState->getCurrentSceneName();
            m_editorState->saveSceneToJson(path);
        } else {
            std::cout << "[DEBUG] Scene name buffer is empty, not saving.\n";
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Load Scene")) {
        if (strlen(sceneNameBuffer) > 0) {
            std::string path = std::string("scenes/") + sceneNameBuffer;
            m_editorState->loadSceneFromJson(path);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear Scene")) {
        m_editorState->getObjects().clear();
        std::cout << "[INFO] Scene cleared" << std::endl;
    }

    // Quick load buttons for common scenes
    ImGui::Text("Quick Load:");
    
    // Dynamically list all .json files in the scenes directory
    std::vector<std::string> sceneFiles;
    for (const auto& entry : std::filesystem::directory_iterator("scenes")) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            sceneFiles.push_back(entry.path().filename().string());
        }
    }
    
    // Sort alphabetically for consistent ordering
    std::sort(sceneFiles.begin(), sceneFiles.end());
    
    // Create buttons for each scene file
    for (const auto& sceneFile : sceneFiles) {
        if (ImGui::Button(sceneFile.c_str())) {
            m_editorState->setCurrentSceneName(sceneFile);
            m_editorState->loadSceneFromJson("scenes/" + sceneFile);
        }
        ImGui::SameLine();
    }

    ImGui::End();
}

void EditorUI::renderObjectProperties() {
    if (m_editorState->getSelectedObjectIndex() >= 0 && 
        m_editorState->getSelectedObjectIndex() < static_cast<int>(m_editorState->getObjects().size())) {
        ImGui::Begin("Object Properties");
        auto& obj = m_editorState->getObjects()[m_editorState->getSelectedObjectIndex()];
        
        ImGui::Text("Object: %s", obj.name.c_str());
        
        float pos[2] = { obj.position.x, obj.position.y };
        if (ImGui::DragFloat2("Position", pos, 1.0f)) {
            obj.position = { pos[0], pos[1] };
        }
        
        if (ImGui::DragFloat("Rotation", &obj.rotation, 1.0f)) {
            // Rotation is already updated
        }
        
        float scale[2] = { obj.scale.x, obj.scale.y };
        if (ImGui::DragFloat2("Scale", scale, 0.1f)) {
            obj.scale = { scale[0], scale[1] };
        }
        
        if (ImGui::Button("Delete Object")) {
            m_editorState->getObjects().erase(m_editorState->getObjects().begin() + m_editorState->getSelectedObjectIndex());
            m_editorState->setSelectedObjectIndex(-1);
        }
        
        ImGui::End();
    }
}

void EditorUI::handleKeyboardInput() {
    // Escape key: exit placement mode or deselect object
    if (glfwGetKey(m_editorState->getEngine()->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (m_editorState->isInPlacementMode()) {
            // Exit placement mode
            m_editorState->setPlacementMode(false);
            m_editorState->setSelectedAsset(-1);
        } else {
            // Deselect object
            m_editorState->setSelectedObjectIndex(-1);
        }
    }
    
    // Delete selected object with Delete key
    if (glfwGetKey(m_editorState->getEngine()->getWindow(), GLFW_KEY_DELETE) == GLFW_PRESS && 
        m_editorState->getSelectedObjectIndex() >= 0 && 
        m_editorState->getSelectedObjectIndex() < static_cast<int>(m_editorState->getObjects().size())) {
        m_editorState->getObjects().erase(m_editorState->getObjects().begin() + m_editorState->getSelectedObjectIndex());
        m_editorState->setSelectedObjectIndex(-1);
    }
}

void EditorUI::handleMouseInput() {
    auto& io = ImGui::GetIO();
    auto camera = m_editorState->getCamera();
    auto engine = m_editorState->getEngine();
    
    // Pan with RMB drag
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right) && !io.WantCaptureMouse) {
        ImVec2 delta = io.MouseDelta;
        camera->pan(glm::vec2(-delta.x, delta.y));
    }
    
    // Zoom with scroll wheel
    if (!io.WantCaptureMouse && io.MouseWheel != 0.0f) {
        double mx, my;
        glfwGetCursorPos(engine->getWindow(), &mx, &my);
        
        int winWidth, winHeight;
        glfwGetWindowSize(engine->getWindow(), &winWidth, &winHeight);
        
        float zoomFactor = (io.MouseWheel > 0) ? 1.1f : 0.9f;
        glm::vec2 screenPoint = { float(mx), float(winHeight - my) };
        camera->zoomAtScreen(zoomFactor, screenPoint);
    }
    
    // Handle object placement and selection
    handleObjectPlacement();
    
    // Update placement preview position
    if (m_editorState->isInPlacementMode() && !ImGui::GetIO().WantCaptureMouse) {
        glm::vec2 world = getMouseWorldPosition();
        m_editorState->setPlacementPreviewPos(world);
    }
}

void EditorUI::handleObjectPlacement() {
    bool isMouseDown = glfwGetMouseButton(m_editorState->getEngine()->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    static bool wasMouseDown = false;

    // Only handle on click-down
    if (isMouseDown && !wasMouseDown && !ImGui::GetIO().WantCaptureMouse && !ImGui::GetIO().WantCaptureKeyboard) {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
            wasMouseDown = isMouseDown;
            return;
        }

        glm::vec2 world = getMouseWorldPosition();
        bool objectSelected = false;

        // Always try to select objects first (regardless of selectedAsset)
        const auto& objects = m_editorState->getObjects();
        for (int i = 0; i < objects.size(); ++i) {
            const auto& obj = objects[i];
            if (isObjectUnderMouse(obj, world)) {
                m_editorState->setSelectedObjectIndex(i);
                objectSelected = true;
                break;
            }
        }

        // If no object selected and we're in placement mode, place the object
        if (!objectSelected && m_editorState->isInPlacementMode()) {
            SceneObject obj;
            const auto& assetPalette = m_editorState->getAssetPalette();
            obj.name = assetPalette[m_editorState->getSelectedAsset()].name;

            // Center on mouse
            const auto& asset = assetPalette[m_editorState->getSelectedAsset()];
            auto tex = m_editorState->getSpriteAtlas()->getTexture();
            if (tex) {
                int texW = tex->m_width;
                int texH = tex->m_height;
                glm::vec2 size = {
                    asset.frame.uvRect.z * texW,
                    asset.frame.uvRect.w * texH
                };
                obj.position = world - 0.5f * size;
            }
            else {
                obj.position = world;
            }
            obj.rotation = 0.0f;
            obj.scale = { 1, 1 };
            obj.assetId = m_editorState->getSelectedAsset();
            m_editorState->getObjects().push_back(obj);
            
            // Exit placement mode after placing
            m_editorState->setPlacementMode(false);
            m_editorState->setSelectedAsset(-1);
        }
        // If no object selected and not in placement mode, just deselect
        else if (!objectSelected) {
            m_editorState->setSelectedObjectIndex(-1);
        }
    }
    wasMouseDown = isMouseDown;
}

bool EditorUI::isMouseOverUI() const {
    return ImGui::GetIO().WantCaptureMouse || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}

glm::vec2 EditorUI::getMouseWorldPosition() const {
    double mx, my;
    glfwGetCursorPos(m_editorState->getEngine()->getWindow(), &mx, &my);
    
    int winWidth, winHeight;
    glfwGetWindowSize(m_editorState->getEngine()->getWindow(), &winWidth, &winHeight);
    
    glm::vec2 screen = { float(mx), float(winHeight - my) };
    return m_editorState->getCamera()->screenToWorld(screen);
}

bool EditorUI::isObjectUnderMouse(const SceneObject& obj, const glm::vec2& mouseWorldPos) const {
    if (obj.assetId < 0 || obj.assetId >= static_cast<int>(m_editorState->getAssetPalette().size())) {
        return false;
    }
    
    const auto& asset = m_editorState->getAssetPalette()[obj.assetId];
    auto tex = m_editorState->getSpriteAtlas()->getTexture();
    if (!tex) return false;
    
    int texW = tex->m_width;
    int texH = tex->m_height;
    glm::vec2 size = {
        asset.frame.uvRect.z * texW,
        asset.frame.uvRect.w * texH
    };

    glm::vec2 objScreenPos = m_editorState->getCamera()->worldToScreen(obj.position);
    glm::vec2 mouseScreenPos = m_editorState->getCamera()->worldToScreen(mouseWorldPos);
    glm::vec2 halfSize = 0.5f * size * m_editorState->getCamera()->getZoom();

    return (mouseScreenPos.x >= objScreenPos.x - halfSize.x && mouseScreenPos.x <= objScreenPos.x + halfSize.x &&
            mouseScreenPos.y >= objScreenPos.y - halfSize.y && mouseScreenPos.y <= objScreenPos.y + halfSize.y);
}

#endif // CH_EDITOR 