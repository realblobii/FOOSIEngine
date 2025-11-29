#include "game/main.h"
#include <iostream>

Engine *engine = nullptr;

int main(int argc, const char *argv[])
{
  engine = new Engine{};
  engine->Init("New Game", 1920, 1080, true);
  engine->loadTileMap("assets/grass_grid.json",64,64);

  while (engine->running())
  {
    
    engine->getDeltaT();
    engine->handleEvents();
    engine->update(engine->deltaTime);
    engine->render();
  }

  engine->clean();
}
