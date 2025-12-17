#include "engine/input/mouse.h"
void mListener::listen(SDL_Event &event){
   if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP){
       cClick.x = event.button.x;
       cClick.y = event.button.y;
       cClick.btn = event.button.button;
       switch(event.type){
           case SDL_MOUSEBUTTONDOWN:
               cClick.state = 1; 
               break;
           case SDL_MOUSEBUTTONUP:
               cClick.state = 0; 
               break;
       }
   }
   else{
    cClick.state = -1;
   }
    // broadcast to any registered handlers (deliver a copy)
    for (auto &h : handlers) {
        h(cClick);
    }
}
mListener::click mListener::get(){
    // return a copy of the last click and consume the event so it
    // doesn't trigger repeatedly across frames
    mListener::click tmp = cClick;
    cClick.state = -1;
    return tmp;
}

void mListener::addHandler(const ClickHandler &h){
    handlers.push_back(h);
}