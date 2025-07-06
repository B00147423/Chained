#include "./headers/Engine.h"
#include "./Game/uiStates/DebugEditorState.h"
#include <memory>
//#include "./Game/uiStates/MainMenu.h" 
using namespace Chained;

int main() {
    Engine engine;
    if (!engine.init()) {
        return -1;
    }

    engine.run(std::make_unique<DebugEditorState>(&engine));  // <-- Use DebugEditorState here
    return 0;
}
