#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "./headers/Engine.h"

#ifdef CH_EDITOR
#include "./headers/EditorState.h"
#endif
#include "./Game/uiStates/TestState.h" // Use TestState for release/game mode

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
    // In Release (no CH_EDITOR), run the game state loading from JSON!
    engine.run(std::make_unique<TestState>("scenes/mainMenu"));
#endif

    return 0;
}
