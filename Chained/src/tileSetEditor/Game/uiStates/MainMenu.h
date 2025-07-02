#pragma once
#include "../../headers/GameState.h"
#include "../../headers/spriteRenderer.h"
#include "../../headers/SpriteAtlas.h"
#include "../../headers/AppState.h"
#include "../../headers/Engine.h"
namespace Chained {

    struct MenuButton {
        const char* sliceName;
        glm::vec2   pos;
        glm::vec2   size;
        bool        hovered = false;
        bool        pressed = false;
    };

    class MainMenu : public GameState {
    public:
        explicit MainMenu(Engine* engine);

        void onEnter()  override;
        void onExit()   override;
        void update(float dt) override;
        void render()   override;
    private:
        void handleInput(MenuButton& btn, float projMouseX, float projMouseY, int mouseState);
        void drawButton(const MenuButton& btn);

        Engine* engine = nullptr;
        std::shared_ptr<SpriteRenderer> renderer;
        std::unique_ptr<SpriteAtlas>    atlas;

        // three buttons
        static constexpr glm::vec2 PlayBtnSize{ 183,  93 };
        static constexpr glm::vec2 SettingsBtnSize{ 179,  87 };
        static constexpr glm::vec2 ExitBtnSize{ 175,  81 };

        MenuButton playBtn{ "play_btn",     {300, 200}, PlayBtnSize };
        MenuButton settingsBtn{ "settings_btn", {300, 310}, SettingsBtnSize };
        MenuButton exitBtn{ "exit_btn",     {300, 410}, ExitBtnSize };

        // shared animation values
        float clickScale = 1.0f;

        // colours
        glm::vec3 idleCol{ 1.0f, 1.0f, 1.0f };
        glm::vec3 hoverCol{ 0.7f, 0.7f, 0.7f };
        glm::vec3 pressedCol{ 0.5f, 0.5f, 0.5f };

        float outlinePx = 3.0f;
    };
}

