#include "game/main.h"
#include <iostream>

Engine *engine = nullptr;

int main(int argc, const char *argv[])
{
  engine = new Engine{};
  engine->Init("New Game", 1920, 1080, true);
  engine->objMgr->instantiate("tile", "pTest", 10.0f,10.0f,1.0f);

  // register a click handler (broadcast style) so multiple systems can listen
  if (engine->mLnr) {
    engine->mLnr->addHandler([](const mListener::click &c){
      if (c.state == 0) {
        std::cout << "Mouse Click at: (" << c.x << ", " << c.y << ")\n";
      }
    });
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
