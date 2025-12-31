#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <memory>
#include "./headers/Engine.h"

#ifdef CH_EDITOR
#include "./headers/EditorState.h"
#endif
#include "./Game/uiStates/MainMenu.h" // Use MainMenu as the entry point
#include "Game/uiStates/TestState.h"
//#include "./Game/uiStates/TestState.h" // Use TestState for release/game mode

using namespace Chained;

int main() {
    Engine engine;
    if (!engine.init()) {
        return -1;
    }

#ifdef CH_EDITOR
    // When building in Debug with CH_EDITOR, run the editor!
    engine.run(std::make_unique<EditorState>(&engine));
#else
    // In Release (no CH_EDITOR), start with the main menu!
    //engine.run(std::make_unique<MainMenu>(&engine));
    //engine.run(std::make_unique<TestState>("scenes/hearts.JSON")); // Alternative: directly run TestState
#endif

    return 0;
}
