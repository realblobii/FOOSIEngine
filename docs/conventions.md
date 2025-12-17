# Project conventions & patterns

- Object registration: classes register at static init time using `ObjectFactory::registerClass` (see `engine/tile/tile_oclass.h`).
- Prototype data: `game/assets/objects.json` contains subclasses and an optional `properties` block. Keep only prototype-level defaults there; instance placement belongs to `instantiate()`.
- Input: prefer `onKey*/onMouse*` high-level registration for game logic. Use `addHandler` for global tools.
- API wrappers: use `game/engine_api.h` for convenient script-level calls; these wrappers do not change engine internals.

Style notes
- Engine uses raw pointers for ownership where the engine owns resources (e.g., `objMgr->registry` holds unique_ptr<Object>). When using `Instantiate()` you receive a raw pointer but do not free it.
- Keep JSON keys snake_case to match existing examples (e.g., `obj_class`, `obj_subclass`).

If you'd like, I can add a CONTRIBUTING.md with preferred PR and coding workflows.
