#include "game/main.h"
#include <iostream>
#include <fstream>

Engine *engine = nullptr;

int main(int argc, const char *argv[])
{
  engine = new Engine{};
  engine->Init("New Game", 1920, 1080, true);

  // create simple test scene files and run load/unload

  if (engine && engine->sceneMgr) {
      std::cout << "Loading scene test.scene..." << std::endl;
      auto sd = engine->sceneMgr->loadScene("test.fscn", 0 ,0,0);
      std::cout << "Loaded scene: " << sd.scene_name << " (" << sd.scene_obj_ids.size() << " objects)\n";
      if (!sd.scene_obj_ids.empty()) {
          std::cout << "Object ids:";
          for (int id : sd.scene_obj_ids) std::cout << " " << id;
          std::cout << "\n";
      }

      
  } else {
      std::cout << "SceneManager missing - test skipped." << std::endl;
  }

  while (engine->running())
  {
    engine->handleEvents();
    engine->update();
    engine->render();
  }

  engine->clean();
}

void Update(){}
