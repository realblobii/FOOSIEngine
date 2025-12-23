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

  // Find the player object named "bob" and bind WASD to move it (simple test; not a full controller)
  Object* bob = nullptr;
  for (auto &p : engine->objMgr->registry) {
    if (p && p->objName == "bob") { bob = p.get(); break; }
  }
  if (bob && engine->kLnr) {
    // Move by 1 unit per hold tick; adjust speed as needed
    engine->kLnr->onKeyHold("a", [bob](const kListener::key &k){ bob->ly += 0.05f; bob->lx -= 0.05f; });
    engine->kLnr->onKeyHold("d", [bob](const kListener::key &k){ bob->ly -= 0.05f; bob->lx += 0.05f;});
    engine->kLnr->onKeyHold("w", [bob](const kListener::key &k){ bob->lx -= 0.1f; bob->ly -= 0.1f; });
    engine->kLnr->onKeyHold("s", [bob](const kListener::key &k){ bob->lx += 0.1f; bob->ly += 0.1f;});
    std::cout << "WASD bound to 'bob' object (id=" << bob->id << ")\n";
  } else {
    std::cout << "Warning: 'bob' not found; WASD not bound" << std::endl;
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
