#include "game/main.h"
#include <iostream>

Engine *engine = nullptr;

int main(int argc, const char *argv[])
{
  engine = new Engine{};
  engine->Init("New Game", 1920, 1080, true);\
  engine->loadTileMap("assets/tiles.json",64,64);

  while (engine->running()) {
    engine->handleEvents();
    engine->update();   // no deltaTime
    engine->render();
}


  engine->clean();
}
