﻿#ifdef CH_EDITOR
#include "../headers/EditorState.h"
#include "../headers/SpriteAtlas.h"
#include "../headers/ResourceManager.h"
#include "../headers/SpriteRenderer.h"
#include <memory>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Game/uiStates/TestState.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <algorithm>
#include <box2d/box2d.h>
using json = nlohmann::json;
using namespace Chained;

constexpr int kLeftPanelWidth = 320;

void EditorState::onExit() {}

EditorState::EditorState(Engine* eng)
    : engine(eng)
{
}
void EditorState::onEnter() {
    auto& rm = *ResourceManager::get();
    rm.addSearchPath("assets/shaders");
    rm.addSearchPath("assets/textures");

    std::cout << "[DEBUG] Loading sprite atlas..." << std::endl;
    spriteAtlas = std::make_shared<SpriteAtlas>("assets/textures/sprites.json");
    std::cout << "[DEBUG] Sprite atlas loaded successfully" << std::endl;

    assetPalette.clear();
    for (const auto& [name, slice] : spriteAtlas->getAllSlices()) {
        assetPalette.push_back({ name, slice });
    }
    std::cout << "[DEBUG] Asset palette size: " << assetPalette.size() << std::endl;

    auto shader = rm.loadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
    renderer = std::make_unique<SpriteRenderer>(shader);

    camera = std::make_unique<Chained::Camera>(Engine::SCREEN_WIDTH, Engine::SCREEN_HEIGHT);

    m_shader = shader;

    shader->use();
    shader->setUniform("projection", camera->getProjectionMatrix());
    shader->setUniform("image", 0);

    std::cout << "[DEBUG] EditorState initialization complete" << std::endl;
}

void EditorState::drawCameraBounds() {
    glm::vec2 camPos = camera->getPosition();
    float camZoom = camera->getZoom();
    float viewportW = camera->getViewportWidth();
    float viewportH = camera->getViewportHeight();

    float left = camPos.x;
    float right = camPos.x + viewportW / camZoom;
    float bottom = camPos.y;
    float top = camPos.y + viewportH / camZoom;
    // (draw code omitted: implement if you want)
}

void EditorState::update(float /*dt*/) {
    int winWidth, winHeight;
    glfwGetWindowSize(engine->getWindow(), &winWidth, &winHeight);
    camera->setViewport(winWidth - kLeftPanelWidth, winHeight); // <-- CORRECT!

    // --- ASSETS & SCENE OBJECTS TABS ---
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(kLeftPanelWidth, winHeight));
    ImGui::Begin("Assets & Scene", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginTabBar("MainTabs")) {
        if (ImGui::BeginTabItem("Assets")) {
            // ASSET PALETTE
            ImGui::Text("Assets");
            ImGui::Separator();
            for (int i = 0; i < assetPalette.size(); ++i) {
                const auto& asset = assetPalette[i];
                ImTextureID texId = (ImTextureID)(intptr_t)spriteAtlas->getTexture()->m_id;
                glm::vec4 uv = asset.frame.uvRect;
                uv.y = 1.f - uv.y - uv.w;
                ImVec2 uv0(uv.x, uv.y);
                ImVec2 uv1(uv.x + uv.z, uv.y + uv.w);
                ImGui::PushID(i);
                if (ImGui::Selectable(asset.name.c_str(), i == selectedAsset)) {
                    // Enter placement mode when selecting an asset
                    if (selectedAsset == i) {
                        // If clicking the same asset, exit placement mode
                        selectedAsset = -1;
                        placementMode = false;
                    } else {
                        // Select new asset and enter placement mode
                        selectedAsset = i;
                        placementMode = true;
                    }
                }
                ImGui::SameLine();
                ImGui::Image(texId, ImVec2(24, 24), uv0, uv1);
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Scene Objects")) {
            // SCENE OBJECTS LIST
            for (int i = 0; i < static_cast<int>(objects.size()); ++i) {
                ImGui::PushID(i);
                if (ImGui::Selectable(objects[i].name.c_str(), i == selectedObjectIndex)) {
                    selectedObjectIndex = i;
                }
                ImGui::PopID();
            }

            // --- OBJECT PROPERTIES ---
            ImGui::Separator();
            if (selectedObjectIndex >= 0 && selectedObjectIndex < static_cast<int>(objects.size())) {
                auto& obj = objects[selectedObjectIndex];
                ImGui::Text("Object: %s", obj.name.c_str());
                float pos[2] = { obj.position.x, obj.position.y };
                if (ImGui::DragFloat2("Position", pos, 1.0f)) {
                    obj.position = { pos[0], pos[1] };
                }
                if (ImGui::DragFloat("Rotation", &obj.rotation, 1.0f)) {}
                float scale[2] = { obj.scale.x, obj.scale.y };
                if (ImGui::DragFloat2("Scale", scale, 0.1f)) {
                    obj.scale = { scale[0], scale[1] };
                }
                if (ImGui::Button("Delete Object")) {
                    objects.erase(objects.begin() + selectedObjectIndex);
                    selectedObjectIndex = -1;
                }
            } else {
                ImGui::Text("No object selected.");
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Scene")) {
            // --- SCENE SAVE/LOAD ---
            static char sceneNameBuffer[128] = "";
            ImGui::InputText("Scene Name", sceneNameBuffer, IM_ARRAYSIZE(sceneNameBuffer));
            if (ImGui::Button("Save Scene")) {
                if (strlen(sceneNameBuffer) > 0) {
                    currentSceneName = sceneNameBuffer;
                    std::string path = std::string("scenes/") + currentSceneName;
                    saveSceneToJson(path);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Load Scene")) {
                if (strlen(sceneNameBuffer) > 0) {
                    std::string path = std::string("scenes/") + sceneNameBuffer;
                    loadSceneFromJson(path);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear Scene")) {
                objects.clear();
            }
            ImGui::Text("Quick Load:");
            std::vector<std::string> sceneFiles;
            for (const auto& entry : std::filesystem::directory_iterator("scenes")) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    sceneFiles.push_back(entry.path().filename().string());
                }
            }
            std::sort(sceneFiles.begin(), sceneFiles.end());
            for (const auto& sceneFile : sceneFiles) {
                if (ImGui::Button(sceneFile.c_str())) {
                    currentSceneName = sceneFile;
                    loadSceneFromJson("scenes/" + sceneFile);
                }
                ImGui::SameLine();
            }
            ImGui::NewLine();
            ImGui::Separator();
            ImGui::Text("Controls");
            ImGui::Text("Asset Palette: Click asset to enter placement mode");
            ImGui::Text("Placement Mode: Click in scene to place object");
            ImGui::Text("Left Click: Select object in list");
            ImGui::Text("Right Click + Drag: Pan camera");
            ImGui::Text("Mouse Wheel: Zoom camera");
            ImGui::Text("Escape: Exit placement mode / Deselect object");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();



    // --- CAMERA PAN & ZOOM CONTROLS ---
    auto& io = ImGui::GetIO();
    
    // Pan with RMB drag
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right) && !io.WantCaptureMouse) {
        ImVec2 delta = io.MouseDelta;
        camera->pan(glm::vec2(-delta.x, delta.y));
    }
    
    // Zoom with scroll wheel
    if (!io.WantCaptureMouse && io.MouseWheel != 0.0f) {
        double mx, my;
        glfwGetCursorPos(engine->getWindow(), &mx, &my);
        float zoomFactor = (io.MouseWheel > 0) ? 1.1f : 0.9f;
        glm::vec2 screenPoint = { float(mx), float(winHeight - my) };
        camera->zoomAtScreen(zoomFactor, screenPoint);
    }

    // --- KEYBOARD INPUT ---
    if (glfwGetKey(engine->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (placementMode) {
            // Exit placement mode
            placementMode = false;
            selectedAsset = -1;
        } else {
            // Deselect object
            selectedObjectIndex = -1;
        }
    }

    // --- MOUSE INPUT ---
    bool isMouseDown = glfwGetMouseButton(engine->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    static bool wasMouseDown = false;

    // Only handle on click-down
    if (isMouseDown && !wasMouseDown && !ImGui::GetIO().WantCaptureMouse && !ImGui::GetIO().WantCaptureKeyboard) {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
            wasMouseDown = isMouseDown;
            return;
        }

        double mx, my;
        glfwGetCursorPos(engine->getWindow(), &mx, &my);
        
        // Adjust for left panel - only handle clicks in the scene area
        if (mx < kLeftPanelWidth) {
            wasMouseDown = isMouseDown;
            return;
        }
        
        glm::vec2 screen = { float(mx) - kLeftPanelWidth, float(winHeight - my) };
        glm::vec2 world = camera->screenToWorld(screen);
        bool objectSelected = false;

        // Always try to select objects first (regardless of placement mode)
        for (int i = 0; i < objects.size(); ++i) {
            const auto& obj = objects[i];
            if (isObjectUnderMouse(obj, world)) {
                selectedObjectIndex = i;
                std::cout << "[DEBUG] Selected object: " << obj.name << " at index " << i << std::endl;
                objectSelected = true;
                break;
            }
        }

        // If no object selected and we're in placement mode, place the object
        if (!objectSelected && placementMode) {
            SceneObject obj;
            obj.name = assetPalette[selectedAsset].name;

            // Center on mouse
            const auto& asset = assetPalette[selectedAsset];
            auto tex = spriteAtlas->getTexture();
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
            obj.assetId = selectedAsset;
            objects.push_back(obj);
            
            // Exit placement mode after placing
            placementMode = false;
            selectedAsset = -1;
        }
        // If no object selected and not in placement mode, just deselect
        else if (!objectSelected) {
            selectedObjectIndex = -1;
        }
    }
    wasMouseDown = isMouseDown;
}

void Chained::EditorState::saveSceneToJson(const std::string& filename) {
    namespace fs = std::filesystem;
    fs::path path(filename);

    if (path.extension() != ".json")
        path.replace_extension(".json");
    std::error_code ec;
    fs::create_directories(path.parent_path(), ec);

    json j;
    for (const auto& obj : objects) {
        j["objects"].push_back({
            {"name", obj.name},
            {"position", {obj.position.x, obj.position.y}},
            {"rotation", obj.rotation},
            {"scale", {obj.scale.x, obj.scale.y}},
            {"assetId", obj.assetId}
            });
    }
    if (camera) {
        j["camera"] = {
            {"pos", {camera->getPosition().x, camera->getPosition().y}},
            {"zoom", camera->getZoom()}
        };
    }
    fs::path tmpPath = path;
    tmpPath += ".tmp";
    std::ofstream file(tmpPath, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open file for writing: " << tmpPath << "\n";
        return;
    }
    file << j.dump(4);
    file.close();
    fs::rename(tmpPath, path, ec);
    if (ec) {
        std::cerr << "[ERROR] Could not move temp file to destination: " << ec.message() << "\n";
        return;
    }
    std::cout << "[INFO] Scene saved to " << path << "\n";
}

void Chained::EditorState::loadSceneFromJson(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open scene file: " << filename << std::endl;
        return;
    }

    size_t lastSlash = filename.find_last_of("/\\");
    size_t lastDot = filename.find_last_of(".");
    if (lastSlash != std::string::npos && lastDot != std::string::npos && lastDot > lastSlash) {
        currentSceneName = filename.substr(lastSlash + 1);
    }
    else if (lastDot != std::string::npos) {
        currentSceneName = filename.substr(0, lastDot + 4);
    }
    else {
        currentSceneName = filename;
        if (currentSceneName.find(".json") == std::string::npos) {
            currentSceneName += ".json";
        }
    }

    json j;
    file >> j;

    objects.clear();
    for (const auto& objJson : j["objects"]) {
        SceneObject obj;
        obj.name = objJson["name"].get<std::string>();
        obj.position = { objJson["position"][0].get<float>(), objJson["position"][1].get<float>() };
        obj.rotation = objJson["rotation"].get<float>();
        obj.scale = { objJson["scale"][0].get<float>(), objJson["scale"][1].get<float>() };
        obj.assetId = objJson["assetId"].get<int>();
        objects.push_back(obj);
    }

    if (j.contains("camera")) {
        auto camJson = j["camera"];
        glm::vec2 camPos = { camJson["pos"][0].get<float>(), camJson["pos"][1].get<float>() };
        float camZoom = camJson["zoom"].get<float>();
        camera->setPostion(camPos);
        camera->setZoom(camZoom);
    }

    std::cout << "[INFO] Loaded scene from: " << filename << " with " << objects.size() << " objects" << std::endl;
}

bool EditorState::isObjectUnderMouse(const SceneObject& obj, const glm::vec2& mouseWorldPos) const {
    if (obj.assetId < 0 || obj.assetId >= static_cast<int>(assetPalette.size())) {
        return false;
    }
    
    const auto& asset = assetPalette[obj.assetId];
    auto tex = spriteAtlas->getTexture();
    if (!tex) return false;
    
    int texW = tex->m_width;
    int texH = tex->m_height;
    glm::vec2 size = {
        asset.frame.uvRect.z * texW,
        asset.frame.uvRect.w * texH
    };

    // Get mouse position in screen coordinates
    double mx, my;
    glfwGetCursorPos(engine->getWindow(), &mx, &my);
    glm::vec2 mouseScreenPos = { float(mx), float(my) };
    
    // Get object position in screen coordinates
    glm::vec2 objScreenPos = camera->worldToScreen(obj.position);
    objScreenPos.x += kLeftPanelWidth;
    
    // Calculate the actual size (with scale)
    glm::vec2 scaledSize = size * obj.scale;
    glm::vec2 halfSize = 0.5f * scaledSize;

    return (mouseScreenPos.x >= objScreenPos.x - halfSize.x && mouseScreenPos.x <= objScreenPos.x + halfSize.x &&
            mouseScreenPos.y >= objScreenPos.y - halfSize.y && mouseScreenPos.y <= objScreenPos.y + halfSize.y);
}

void EditorState::render() {
    int winWidth, winHeight;
    glfwGetWindowSize(engine->getWindow(), &winWidth, &winHeight);

    // Restrict rendering to scene area (not the left panel)
    glEnable(GL_SCISSOR_TEST);
    glViewport(kLeftPanelWidth, 0, winWidth - kLeftPanelWidth, winHeight);
    glScissor(kLeftPanelWidth, 0, winWidth - kLeftPanelWidth, winHeight);
    glClearColor(0.05f, 0.05f, 0.05f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader->use();
    m_shader->setUniform("projection", camera->getProjectionMatrix());

    auto tex = spriteAtlas->getTexture();
    if (!tex) return;
    int texW = tex->m_width;
    int texH = tex->m_height;

    for (int i = 0; i < objects.size(); ++i) {
        const auto& obj = objects[i];
        if (obj.assetId < 0 || obj.assetId >= static_cast<int>(assetPalette.size())) continue;
        const auto& asset = assetPalette[obj.assetId];
        glm::vec2 size = {
            asset.frame.uvRect.z * texW,
            asset.frame.uvRect.w * texH
        };
        glm::vec4 uv = asset.frame.uvRect;
        uv.y = 1.f - uv.y - uv.w;
        glm::vec3 color = (i == selectedObjectIndex) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f);

        renderer->DrawSprite(
            tex,
            obj.position,
            size,
            obj.rotation,
            color,
            uv
        );
    }
    drawCameraBounds();

    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, winWidth, winHeight);
}

#endif
