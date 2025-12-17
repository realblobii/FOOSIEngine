#pragma once 
#include <SDL2/SDL_events.h>   
#include <SDL2/SDL_mouse.h>
#include <functional>
#include <vector>

class mListener{
    public:
    // make the click struct visible so callers can use the returned type
    struct click{
        int x;
        int y;
        int state;
        int btn;
    };

    using ClickHandler = std::function<void(const click&)>;

    void listen(SDL_Event &event);
    click get();

    // register a callback to be invoked whenever a click event is received
    void addHandler(const ClickHandler &h);

    private:
    // store the last click event for this listener
    click cClick;

    // registered handlers to broadcast events to
    std::vector<ClickHandler> handlers;
    };