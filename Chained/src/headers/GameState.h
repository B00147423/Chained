#pragma once

namespace Chained {

    class GameState {
    public:
        virtual ~GameState() {}
        virtual void onEnter() = 0;
        virtual void onExit() = 0;
        virtual void update(float dt) = 0;
        virtual void render() = 0;
    };

}
