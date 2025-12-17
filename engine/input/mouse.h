#pragma once 
#include <SDL2/SDL_events.h>   
#include <SDL2/SDL_mouse.h>
#include <functional>
#include <vector>
#include <string>

class mListener{
    public:
    // make the click struct visible so callers can use the returned type
    struct click{
        int x;
        int y;
        int state;
        int btn;
        bool down;
    };

    using ClickHandler = std::function<void(const click&)>;

    void listen(SDL_Event &event);
    click get();

    // legacy broadcast: register a callback invoked for every mouse event
    void addHandler(const ClickHandler &h);

    // convenience per-button handlers. btn may be SDL_BUTTON_LEFT, etc.
    void onMouseDown(int btn, const ClickHandler &h);
    void onMouseUp(int btn, const ClickHandler &h);
    void onMouseHold(int btn, const ClickHandler &h);

    // convenience overloads accepting button names: "left", "right", "middle"
    void onMouseDown(const std::string &btnName, const ClickHandler &h);
    void onMouseUp(const std::string &btnName, const ClickHandler &h);
    void onMouseHold(const std::string &btnName, const ClickHandler &h);

    // call per-frame to trigger hold handlers (should be called from Engine::update)
    void tick();

    private:
    // store the last click event for this listener
    click cClick;

    // registered handlers to broadcast events to
    std::vector<ClickHandler> handlers;
    // per-button registries
    struct BtnReg { int btn; ClickHandler h; };
    std::vector<BtnReg> downRegs;
    std::vector<BtnReg> upRegs;
    std::vector<BtnReg> holdRegs;
    };