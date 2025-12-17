#include "engine/input/mouse.h"
#include <algorithm>
#include <string>
void mListener::listen(SDL_Event &event){
   if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP){
       cClick.x = event.button.x;
       cClick.y = event.button.y;
       cClick.btn = event.button.button;
       switch(event.type){
           case SDL_MOUSEBUTTONDOWN:
               cClick.state = 1; 
               cClick.down = true;
               break;
           case SDL_MOUSEBUTTONUP:
               cClick.state = 0; 
               cClick.down = false;
               break;
       }
   }
   else{
    cClick.state = -1;
   }
   if (cClick.down){
        SDL_GetMouseState(&cClick.x, &cClick.y);
   }
    // broadcast to any registered handlers (deliver a copy)
    for (auto &h : handlers) {
        h(cClick);
    }

    // call down/up handlers for this event
    if (event.type == SDL_MOUSEBUTTONDOWN){
        for (auto &r : downRegs){ if (r.btn == cClick.btn) r.h(cClick); }
    } else if (event.type == SDL_MOUSEBUTTONUP){
        for (auto &r : upRegs){ if (r.btn == cClick.btn) r.h(cClick); }
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

void mListener::onMouseDown(int btn, const ClickHandler &h){
    downRegs.push_back({btn,h});
}
void mListener::onMouseUp(int btn, const ClickHandler &h){
    upRegs.push_back({btn,h});
}
void mListener::onMouseHold(int btn, const ClickHandler &h){
    holdRegs.push_back({btn,h});
}

void mListener::onMouseDown(const std::string &btnName, const ClickHandler &h){
    int b = 0;
    std::string s = btnName;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "left") b = SDL_BUTTON_LEFT;
    else if (s == "right") b = SDL_BUTTON_RIGHT;
    else if (s == "middle") b = SDL_BUTTON_MIDDLE;
    if (b) onMouseDown(b,h);
}
void mListener::onMouseUp(const std::string &btnName, const ClickHandler &h){
    int b = 0;
    std::string s = btnName;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "left") b = SDL_BUTTON_LEFT;
    else if (s == "right") b = SDL_BUTTON_RIGHT;
    else if (s == "middle") b = SDL_BUTTON_MIDDLE;
    if (b) onMouseUp(b,h);
}
void mListener::onMouseHold(const std::string &btnName, const ClickHandler &h){
    int b = 0;
    std::string s = btnName;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "left") b = SDL_BUTTON_LEFT;
    else if (s == "right") b = SDL_BUTTON_RIGHT;
    else if (s == "middle") b = SDL_BUTTON_MIDDLE;
    if (b) onMouseHold(b,h);
}

void mListener::tick(){
    // poll mouse state and call hold handlers for matching buttons
    int x=0,y=0;
    Uint32 state = SDL_GetMouseState(&x,&y);
    for (auto &r : holdRegs){
        if (state & SDL_BUTTON(r.btn)){
            click c = cClick; // copy
            c.x = x; c.y = y; c.down = true;
            r.h(c);
        }
    }
}