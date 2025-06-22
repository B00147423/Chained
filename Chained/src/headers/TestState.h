// TestState.h
#pragma once
#include "GameState.h"
#include "SpriteAtlas.h"
#include "spriteRenderer.h"
#include "resourceManager.h"

namespace Chained {

    class TestState : public GameState {
    public:
        void onEnter() override;
        void onExit() override;
        void update(float dt) override;
        void render() override;

    private:
        std::shared_ptr<SpriteRenderer> renderer;
        std::unique_ptr<SpriteAtlas> atlas;
    };

}
