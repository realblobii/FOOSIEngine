#include "game/main.h"
#include <iostream>

Engine *engine = nullptr;

int main(int argc, const char *argv[])
{
  engine = new Engine{};
  engine->Init("New Game", 1920, 1080, true);

  while (engine->running())
  {
    
    engine->handleEvents();
    engine->update();
    engine->render();
  }

  engine->clean();
}

void Update(){}
