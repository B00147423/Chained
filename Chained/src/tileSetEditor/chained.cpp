﻿#include "./headers/Engine.h"
//#include "./Game/uiStates/MainMenu.h" 
using namespace Chained;

int main() {
    Engine engine;
    if (!engine.init()) {
        return -1;
    }

    engine.run(std::make_unique<EditorState>(&engine));  // <-- Use EditorState here
    return 0;
}
