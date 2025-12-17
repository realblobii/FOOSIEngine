#pragma once
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <functional>
#include <vector>
#include <string>

class kListener {
public:
    // keyboard event visible to callers
    struct key {
        int key;    // SDL_Keycode (cast to int)
        int state;  // 1 = down, 0 = up, -1 = none
        bool down;  // true while key is held
    };

    using KeyHandler = std::function<void(const key&)>;

    // handle an incoming SDL_Event (call from your SDL poll loop)
    void listen(SDL_Event &event);

    // poll-style accessor (returns copy and consumes the stored state)
    key get();

    // register a callback to be invoked whenever a key event is received
    void addHandler(const KeyHandler &h);

    // convenience per-key handlers. keycode is SDL_Keycode (e.g. SDLK_UP)
    void onKeyDown(SDL_Keycode key, const KeyHandler &h);
    void onKeyUp(SDL_Keycode key, const KeyHandler &h);
    void onKeyHold(SDL_Keycode key, const KeyHandler &h);

    // convenience overloads accepting key names, e.g. "up", "a", "space"
    void onKeyDown(const std::string &keyName, const KeyHandler &h);
    void onKeyUp(const std::string &keyName, const KeyHandler &h);
    void onKeyHold(const std::string &keyName, const KeyHandler &h);

    // call per-frame to trigger hold handlers (should be called from Engine::update)
    void tick();

private:
    // store the last key event for this listener
    key cKey{0, -1, false};

    // registered handlers to broadcast events to
    std::vector<KeyHandler> handlers;
    struct KeyReg { SDL_Keycode key; KeyHandler h; };
    std::vector<KeyReg> downRegs;
    std::vector<KeyReg> upRegs;
    std::vector<KeyReg> holdRegs;
};
