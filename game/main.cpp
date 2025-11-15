#include "game/main.h"
#include <iostream>
Engine *engine = nullptr;  
int main(int argc, const char * argv[]){
      engine = new Engine{};
      engine->Init("New Game", 1080,1920,true);

      while (engine->running()){
        engine->handleEvents();
        engine->render();
        engine->update();
      }
    engine->clean();
}
void Update(){
}
