#include "game/main.h"
#include "engine_api.h"
#include <iostream>
#include <fstream>

Engine *engine = nullptr;

int main(int argc, const char *argv[])
{
  engine = new Engine{};
  // No args required: engine reads configuration from game/foosiecfg.json by default
  engine->Init();

  // create simple test scene files and run load/unload

  loadScene("test.fscn");

  while (engine->running())
  {
    engine->handleEvents();
    engine->update();
    engine->render();
  }

  engine->clean();
}

void Update(){}
