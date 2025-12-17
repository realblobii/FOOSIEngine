#include "engine/input/keyboard.h"

#include <string>
#include <algorithm>

void kListener::listen(SDL_Event &event){
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP){
        cKey.key = static_cast<int>(event.key.keysym.sym);
        switch(event.type){
            case SDL_KEYDOWN:
                cKey.state = 1;
                cKey.down = true;
                break;
            case SDL_KEYUP:
                cKey.state = 0;
                cKey.down = false;
                break;
        }
    } else {
        cKey.state = -1;
    }

    // broadcast to any registered handlers (deliver a copy)
    for (auto &h : handlers) {
        h(cKey);
    }

    // call down/up handlers for this event
    if (event.type == SDL_KEYDOWN){
        for (auto &r : downRegs){ if (r.key == cKey.key) r.h(cKey); }
    } else if (event.type == SDL_KEYUP){
        for (auto &r : upRegs){ if (r.key == cKey.key) r.h(cKey); }
    }
}

kListener::key kListener::get(){
    // return a copy of the last key and consume the event so it
    // doesn't trigger repeatedly across frames
    kListener::key tmp = cKey;
    cKey.state = -1;
    return tmp;
}

void kListener::addHandler(const KeyHandler &h){
    handlers.push_back(h);
}

void kListener::onKeyDown(SDL_Keycode key, const KeyHandler &h){
    downRegs.push_back({key,h});
}
void kListener::onKeyUp(SDL_Keycode key, const KeyHandler &h){
    upRegs.push_back({key,h});
}
void kListener::onKeyHold(SDL_Keycode key, const KeyHandler &h){
    holdRegs.push_back({key,h});
}

void kListener::onKeyDown(const std::string &keyName, const KeyHandler &h){
    SDL_Keycode kc = SDL_GetKeyFromName(keyName.c_str());
    if (kc != SDLK_UNKNOWN) onKeyDown(kc,h);
}
void kListener::onKeyUp(const std::string &keyName, const KeyHandler &h){
    SDL_Keycode kc = SDL_GetKeyFromName(keyName.c_str());
    if (kc != SDLK_UNKNOWN) onKeyUp(kc,h);
}
void kListener::onKeyHold(const std::string &keyName, const KeyHandler &h){
    SDL_Keycode kc = SDL_GetKeyFromName(keyName.c_str());
    if (kc != SDLK_UNKNOWN) onKeyHold(kc,h);
}

void kListener::tick(){
    // poll keyboard state and call hold handlers for matching keys
    int numKeys;
    const Uint8 *state = SDL_GetKeyboardState(&numKeys);
    for (auto &r : holdRegs){
        SDL_Scancode sc = SDL_GetScancodeFromKey(r.key);
        if (sc >= 0 && sc < numKeys && state[sc]){
            key k = cKey; // copy
            k.down = true;
            k.key = static_cast<int>(r.key);
            r.h(k);
        }
    }
}
