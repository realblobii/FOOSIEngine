# Object system & prototypes

Location: `engine/obj/*`, `game/assets/objects.json`

Key concepts
- Classes vs Subclasses:
  - A *class* maps to a C++ type (registered via `ObjectFactory::registerClass()`).
  - A *subclass* is a JSON prototype: data-driven variant of a class (textures, properties).

Factory & Registry
- `ObjectFactory::registerClass(name, creator)` registers class creators.
- `objManager::instantiate(class, subclass, x,y,z)` creates an instance, assigns id/position, applies prototype properties (see below), and resolves textures. The instance is stored in `objManager::registry` and returned as a raw pointer (owned by the registry).

Prototype `properties`
- `game/assets/objects.json` may include a `properties` object per prototype.
- **Important**: core engine-managed fields (`x`,`y`,`z`,`texref`) are ignored when set in prototypes. The engine will emit a warning and strip them.
- To expose prototype-driven variables, classes must register named property setters:
  - In a class constructor, call `registerFloatProperty("speed", speed);` or `registerStringProperty("team", teamName);`
  - For custom parsing, use `registerProperty("loot", [this](const Json::Value &v){ /* parse */ });`

Example prototype:

```json
{
  "obj_class": "tile",
  "obj_subclass": "pPlayer",
  "textures": { "default": "assets/player.png" },
  "properties": { "speed": 2.0 }
}
```

Extending classes
- Override `applyProperties` only if you need custom behavior; the preferred pattern is to register setters in the constructor.

Why this design
- Keeps engine-managed concerns separated from data-driven prototype properties.
- Enables safe, explicit exposure of variables to JSON while preventing accidental overriding of instance placement or texture resolution.
