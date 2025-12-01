#include "game/main.h"
#include <iostream>

Engine *engine = nullptr;

int main(int argc, const char *argv[])
{
  engine = new Engine{};
  engine->Init("New Game", 1920, 1080, true);
  engine->loadTileMap("assets/grass_grid.json",64,64);
  engine->objMgr->instantiate("tile", "pTest", 15,15,1);

  while (engine->running())
  {
    
  
    engine->handleEvents();
    engine->update();
    engine->render();
  }

  engine->clean();
}
