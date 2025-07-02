#ifdef CH_EDITOR
#include "../headers/EditorState.h"
#include "../headers/SpriteAtlas.h"
#include "../headers/ResourceManager.h"
#include "../headers/SpriteRenderer.h"
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Game/uiStates/TestState.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>           // for std::ofstream
#include <nlohmann/json.hpp>
#include <filesystem>
#include <algorithm>
using json = nlohmann::json;
using namespace Chained;

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

    // CAMERA: Construct with initial viewport size, will be updated in update()
    camera = std::make_unique<Chained::Camera>(Engine::SCREEN_WIDTH, Engine::SCREEN_HEIGHT);

    // Store shader reference for updating projection matrix
    m_shader = shader;

    // Use camera's projection for the shader
    shader->use();
    shader->setUniform("projection", camera->getProjectionMatrix());
    shader->setUniform("image", 0);

    std::cout << "[DEBUG] EditorState initialization complete" << std::endl;
}

// Draws a yellow rectangle showing the camera's current visible area in world space
void EditorState::drawCameraBounds() {
    glm::vec2 camPos = camera->getPosition();
    float camZoom = camera->getZoom();
    float viewportW = camera->getViewportWidth();
    float viewportH = camera->getViewportHeight();

    float left = camPos.x;
    float right = camPos.x + viewportW / camZoom;
    float bottom = camPos.y;
    float top = camPos.y + viewportH / camZoom;

    // Deprecated OpenGL code commented out for compatibility with modern OpenGL
    /*
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_TEXTURE_2D); // No texture
    //glDisable(GL_BLEND);      // No blend

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(glm::value_ptr(camera->getProjectionMatrix()));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glLineWidth(2.0f);
    glColor3f(1.0f, 1.0f, 0.2f); // Yellow

    glBegin(GL_LINE_LOOP);
    glVertex2f(left, bottom);
    glVertex2f(right, bottom);
    glVertex2f(right, top);
    glVertex2f(left, top);
    glEnd();

    glPopMatrix(); // MODELVIEW
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
    */
}
void EditorState::update(float /*dt*/) {
    // --- UPDATE CAMERA VIEWPORT TO MATCH WINDOW SIZE ---
    int winWidth, winHeight;
    glfwGetWindowSize(engine->getWindow(), &winWidth, &winHeight);
    camera->setViewport(winWidth, winHeight);
    
    // --- SETUP MAIN DOCKSPACE (Unity/VSCode style) ---
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
        // Pass the mouse coordinates in the same coordinate system as screenToWorld
        glm::vec2 screenPoint = { float(mx), float(winHeight - my) };
        camera->zoomAtScreen(zoomFactor, screenPoint);
    }

    // Deselect object with Escape key
    if (glfwGetKey(engine->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        selectedObjectIndex = -1;
    }

    // --- ASSET PALETTE WINDOW ---
    ImGui::Begin("Assets");
    for (int i = 0; i < assetPalette.size(); ++i) {
        const auto& asset = assetPalette[i];
        ImTextureID texId = (ImTextureID)(intptr_t)spriteAtlas->getTexture()->m_id;
        glm::vec4 uv = asset.frame.uvRect;
        uv.y = 1.f - uv.y - uv.w;
        ImVec2 uv0(uv.x, uv.y);
        ImVec2 uv1(uv.x + uv.z, uv.y + uv.w);
        ImGui::PushID(i);
        if (ImGui::Selectable(asset.name.c_str(), i == selectedAsset)) {
            selectedAsset = i;
        }
        ImGui::SameLine();
        ImGui::Image(texId, ImVec2(24, 24), uv0, uv1);
        ImGui::PopID();
    }
    ImGui::End();

    // --- "Test" BUTTON (Switch to TestState with camera and objects) ---
    ImGui::Begin("Editor Controls");
    if (ImGui::Button("Test")) {
        saveSceneToJson("scenes/temp_test.json");
        engine->run(std::make_unique<TestState>("scenes/temp_test.json"));
    }
    ImGui::End();
    
    // --- CONTROLS HELP ---
    ImGui::Begin("Controls");
    ImGui::Text("Left Click: Place new object");
    ImGui::Text("Ctrl + Left Click: Select existing object");
    ImGui::Text("Right Click + Drag: Pan camera");
    ImGui::Text("Mouse Wheel: Zoom camera");
    ImGui::Text("Escape: Deselect object");
    ImGui::End();

    ImGui::Begin("Scene Save/Load");

    // At the top of update (or as a static in the function):
    static char sceneNameBuffer[128] = "";

    // In the ImGui code:
    ImGui::InputText("Scene Name", sceneNameBuffer, IM_ARRAYSIZE(sceneNameBuffer));

    if (ImGui::Button("Save Scene")) {
        std::cout << "[DEBUG] Save Scene button pressed, buffer: '" << sceneNameBuffer << "'\n";
        if (strlen(sceneNameBuffer) > 0) {
            currentSceneName = sceneNameBuffer; // Update the current scene name
            std::string path = std::string("scenes/") + currentSceneName;
            saveSceneToJson(path);
        } else {
            std::cout << "[DEBUG] Scene name buffer is empty, not saving.\n";
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Load Scene")) {
        if (strlen(sceneNameBuffer) > 0) { // Only load if a name is entered
            std::string path = std::string("scenes/") + sceneNameBuffer;
            loadSceneFromJson(path);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear Scene")) {
        objects.clear();
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
            currentSceneName = sceneFile;
            loadSceneFromJson("scenes/" + sceneFile);
        }
        ImGui::SameLine();
    }

    ImGui::End();
    // --- PLACE OBJECT ON CLICK ---
    bool isMouseDown = glfwGetMouseButton(engine->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    static bool wasMouseDown = false;
    bool isCtrlPressed = glfwGetKey(engine->getWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || 
                        glfwGetKey(engine->getWindow(), GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
    
    if (isMouseDown && !wasMouseDown && !ImGui::GetIO().WantCaptureMouse && !ImGui::GetIO().WantCaptureKeyboard) {
        // Additional check: make sure we're not clicking on any ImGui window
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
            wasMouseDown = isMouseDown;
            return;
        }
        
        // 2. Get mouse position relative to the scene area
        double mx, my;
        glfwGetCursorPos(engine->getWindow(), &mx, &my);

        // 3. Use window size for Y flip
        glm::vec2 screen = { float(mx), float(winHeight - my) };

        // 4. Use this for screenToWorld
        glm::vec2 world = camera->screenToWorld(screen);

        // Only try to select objects if Ctrl is held down
        bool objectSelected = false;
        if (isCtrlPressed) {
            for (int i = 0; i < objects.size(); ++i) {
                auto& obj = objects[i];
                if (obj.assetId >= 0 && obj.assetId < static_cast<int>(assetPalette.size())) {
                    const auto& asset = assetPalette[obj.assetId];
                    auto tex = spriteAtlas->getTexture();
                    if (tex) {
                        int texW = tex->m_width;
                        int texH = tex->m_height;
                        glm::vec2 size = {
                            asset.frame.uvRect.z * texW,
                            asset.frame.uvRect.w * texH
                        };
                        
                        // Convert object position to screen coordinates for comparison
                        glm::vec2 objScreenPos = camera->worldToScreen(obj.position);
                        glm::vec2 halfSize = 0.5f * size * camera->getZoom();
                        
                        // Check if mouse click is within the object bounds
                        if (screen.x >= objScreenPos.x - halfSize.x && screen.x <= objScreenPos.x + halfSize.x &&
                            screen.y >= objScreenPos.y - halfSize.y && screen.y <= objScreenPos.y + halfSize.y) {
                            selectedObjectIndex = i;
                            objectSelected = true;
                            break;
                        }
                    }
                }
            }
        }

        // If no object selected, place new object
        if (!objectSelected) {
            selectedObjectIndex = -1; // Deselect
            SceneObject obj;
            obj.name = assetPalette[selectedAsset].name;
            
            // Center the object on the mouse position
            if (selectedAsset >= 0 && selectedAsset < static_cast<int>(assetPalette.size())) {
                const auto& asset = assetPalette[selectedAsset];
                auto tex = spriteAtlas->getTexture();
                if (tex) {
                    int texW = tex->m_width;
                    int texH = tex->m_height;
                    glm::vec2 size = {
                        asset.frame.uvRect.z * texW,
                        asset.frame.uvRect.w * texH
                    };
                    // Adjust position to center the object on the mouse
                    obj.position = world - 0.5f * size;
                } else {
                    obj.position = world;
                }
            } else {
                obj.position = world;
            }
            
            obj.rotation = 0.0f;
            obj.scale = { 1, 1 };
            obj.assetId = selectedAsset;
            objects.push_back(obj);
        }
    }
    wasMouseDown = isMouseDown;

    // --- OBJECT PROPERTIES WINDOW ---
    if (selectedObjectIndex >= 0 && selectedObjectIndex < static_cast<int>(objects.size())) {
        ImGui::Begin("Object Properties");
        auto& obj = objects[selectedObjectIndex];
        
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
            objects.erase(objects.begin() + selectedObjectIndex);
            selectedObjectIndex = -1;
        }
        
        ImGui::End();
    }
}

void Chained::EditorState::saveSceneToJson(const std::string& filename) {
    namespace fs = std::filesystem;
    fs::path path(filename);

    // Always ensure .json extension
    if (path.extension() != ".json")
        path.replace_extension(".json");

    // Make sure parent directory exists
    std::error_code ec;
    fs::create_directories(path.parent_path(), ec);
    if (ec) {
        std::cerr << "[ERROR] Could not create directory: " << path.parent_path()
            << " (" << ec.message() << ")\n";
        return;
    }

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

    // Write atomically to temp then move (optional but good practice)
    fs::path tmpPath = path;
    tmpPath += ".tmp";
    std::ofstream file(tmpPath, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open file for writing: " << tmpPath << "\n";
        return;
    }
    file << j.dump(4);
    file.close();

    // Atomic replace
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

    // Extract scene name from filename
    size_t lastSlash = filename.find_last_of("/\\");
    size_t lastDot = filename.find_last_of(".");
    if (lastSlash != std::string::npos && lastDot != std::string::npos && lastDot > lastSlash) {
        currentSceneName = filename.substr(lastSlash + 1); // Include the .json extension
    } else if (lastDot != std::string::npos) {
        currentSceneName = filename.substr(0, lastDot + 4); // Include the .json extension
    } else {
        currentSceneName = filename;
        // Add .json extension if it's missing
        if (currentSceneName.find(".json") == std::string::npos) {
            currentSceneName += ".json";
        }
    }

    json j;
    file >> j;

    // Clear existing objects
    objects.clear();

    // Load objects
    for (const auto& objJson : j["objects"]) {
        SceneObject obj;
        obj.name = objJson["name"].get<std::string>();
        obj.position = { objJson["position"][0].get<float>(), objJson["position"][1].get<float>() };
        obj.rotation = objJson["rotation"].get<float>();
        obj.scale = { objJson["scale"][0].get<float>(), objJson["scale"][1].get<float>() };
        obj.assetId = objJson["assetId"].get<int>();
        objects.push_back(obj);
    }

    // Load camera info if present
    if (j.contains("camera")) {
        auto camJson = j["camera"];
        glm::vec2 camPos = { camJson["pos"][0].get<float>(), camJson["pos"][1].get<float>() };
        float camZoom = camJson["zoom"].get<float>();
        camera->setPostion(camPos);
        camera->setZoom(camZoom);
    }

    std::cout << "[INFO] Loaded scene from: " << filename << " with " << objects.size() << " objects" << std::endl;
}

void EditorState::render() {
    // Update shader projection matrix to match current camera state
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
        // Use different color for selected object
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
}


#endif