#include "./headers/Engine.h"
#include "./headers/MainMenu.h"  // ✅ include MainMenu instead
using namespace Chained;

int main() {
    Engine engine;
    if (!engine.init()) {
        return -1;
    }

    engine.run(std::make_unique<MainMenu>(&engine));  // ✅ Start with MainMenu
    return 0;
}
