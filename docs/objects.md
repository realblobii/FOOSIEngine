# Object system & prototypes

Location: `engine/obj/*`, `game/assets/objects.json`

Purpose
- Describe object classes, prototype-driven properties (`objects.json`), object creation, and the runtime registry.

## Key concepts
### - Classes vs Subclasses:
  - A *class* maps to a C++ type (registered via `ObjectFactory::registerClass()`).
  - A *subclass* is a JSON prototype: data-driven variant of a class (textures, properties).

## Factory & Registry
- `ObjectFactory::registerClass(name, creator)` registers class creators.
- `objManager::instantiate(class, subclass, x,y,z)` creates an instance, assigns id/position, applies prototype properties (see below), and resolves textures. The instance is stored in `objManager::registry` and returned as a raw pointer (owned by the registry).

## Prototype `properties`
- `game/assets/objects.json` may include a `properties` object per prototype.
- **Important**: core engine-managed fields (`x`,`y`,`z`,`texref`) are ignored when set in prototypes. The engine will emit a warning and strip them.

### Engine validation
- The engine requires that any `OBJECT` referenced in scene files corresponds to a prototype present in `game/assets/objects.json` (i.e. valid `obj_class` and `obj_subclass`). If a prototype is missing, `Instantiate()` will return `nullptr` and the loader will skip the entry. Ensure your scene files only reference defined subclasses (for example: `tile.grass`, `tile.half_grass`, `tile.pTest`).
- To expose prototype-driven variables, classes must register named property setters:
  - In a class constructor, call `registerFloatProperty("speed", speed);` or `registerStringProperty("team", teamName);`
  - For custom parsing, use `registerProperty("loot", [this](const Json::Value &v){ /* parse */ });`

### Example prototype:

```json
{
  "obj_class": "tile",
  "obj_subclass": "pPlayer",
  "textures": { "default": "assets/player.png" },
  "properties": { "speed": 2.0 }
}
```

## Extending classes
- Override `applyProperties` only if you need custom behavior; the preferred pattern is to register setters in the constructor.

## UI objects (ui.text)

- The engine includes a built-in `ui.text` subclass for screen-space labels that are rendered by the GuiLayer (not as world-space textures).
- Class: `UIText_OBJ` (registered as `ui.text`). Properties available in prototypes and scene files:
  - `text` (string) — the literal text to display.
  - `font` (string) — path to a TTF file (e.g., `demo/fonts/DMSans.ttf`) or empty to use the default discovered font.
  - `size` (int) — pixel font size.
  - `nx`, `ny` (float) — normalized (0..1) coordinates mapping to NDC for screen placement (preferred for `UI` syntax). If you supply `sx`/`sy` instead, those are treated as pixel positions.
  - `sx`, `sy` (float) — pixel coordinates (screen space); used if `nx/ny` are negative.
  - `r`, `g`, `b`, `a` (float) — color components in [0..1].

- Important: `ui.*` objects do not attempt engine texture resolution (they are not looked up in `objects.json` texture maps). The GuiLayer manages font rendering and glyph atlases directly.

- Creation: use scene `UI` syntax (see `docs/scene.md`) or the convenience helper `InstantiateUIText()` in `game/engine_api.h`.


## Object Manipulation
### Creation
  -- `Object* Instantiate(const string &class, const string &subclass, float x, float y, float z)`
    - Script-friendly wrapper for instantiating prototypes. Prefer calling this helper from game code instead of accessing engine internals.
    - Returns a raw pointer owned by the engine registry.
### Hierarchy Manipulation
  -- `void objManager::addChild(Object* parent, Object* child)`
   - Makes `child` a child of `parent` and removes libks between `child` and previous parents.
  -- `void objManager::removeChild(Object* parent, Object* child)`
     - Makes `child` no longer a child of `parent`.
     - `child` becomes a child of the `ROOT` object.
  -- `Object* Object::getParent()`
   - Gets the parent of the `Object` chosen. 
### Registry 
  -- `void objManager::printTree(Object* obj, const std::string& prefix = "", bool isLast = true)`
   -  Prints out entire hierarchy from `obj`.
  -- `void objManager::printRegistry() const`
   - Prints out entire registry, ignoring hierarchy.
  -- `Object* objManager::getRoot() const`
   - Returns the `ROOT` object, parent of all other objects. 

## Why this design
- Keeps engine-managed concerns separated from data-driven prototype properties.
- Enables safe, explicit exposure of variables to JSON while preventing accidental overriding of instance placement or texture resolution.
