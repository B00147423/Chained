#pragma once

#include "../../headers/GameState.h"
#include "../../headers/AppState.h"
#include "../../headers/Engine.h"
#include <vector>
#include <memory>

namespace Chained {

    // forward declarations to avoid heavy includes
    class SpriteRenderer;
    class Texture2D;

    class DebugEditorState : public GameState
    {
    public:
        explicit DebugEditorState(Engine* eng);    // constructor

        void onEnter()  override;
        void onExit() override;
        void update(float dt) override;
        void render()   override;
        AppState result = AppState::Editor;   // ← ADD THIS
    private:
        /* ───── minimal tile grid ───── */
        struct TileMap {
            int w = 20;
            int h = 15;
            std::vector<int> tiles;

            TileMap() : tiles(w* h, 0) {}

            int& at(int x, int y) { return tiles[y * w + x]; }
            int  at(int x, int y) const { return tiles[y * w + x]; }

            void save(const char* path) const;
            bool load(const char* path);
        };

        /* members */
        Engine* engine = nullptr;
        TileMap                       map;
        int                           current = 1;        // selected tile id
        std::unique_ptr<SpriteRenderer> renderer;            // draws quads
        std::shared_ptr<Texture2D>    whiteTex;              // 1×1 white
    };

} // namespace Chained
