### Subclass Jargon
Subclasses for the most part aren't meant to define their own code, however, if you really need it, you can always use switch statements in the respective Object Class' code. One thing we plan to add to subclasses in the future is them determining their own variable amounts. While they won't be able to initiate new variables, they will be able to have custom amounts for some of the overarching Object Class' variables (not core engine things like coords or ID though).

### Object Loader
Generally, object classes are created in a JSON file, and defined in a header/CPP file. The JSON file mainly initiates the objects and gives things like textures or variables to sub-classes, whereas the C++ will ensure the main class will work in the engine. If you don't have a class in both JSON and C++, the Object Loader will tend to fail. We plan to automate this in future versions via commandline TUI, or a bespoke GUI.

### Accessing the Registry
The Registry is a child vector of the `objManager` class and so forth can be accessed via `engine->objMgr`. In general prefer the script-facing helper `Instantiate()` (see `game/engine_api.h`) when creating objects; that keeps engine internals encapsulated. If you must access `engine->objMgr` directly, consider copying the registry before mutating it to avoid upsetting object logic.

### Adding/Removing objects
So far, adding objects is as easy as `objManager::instantiate()` for your object! Though, I would recommend using Tilemaps for `tile` objects. Again, this will be updated when I add a simple registry object destroyer. Project is very WIP at the moment. 
