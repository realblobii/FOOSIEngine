# UI & GuiLayer

Location: `engine/foogui/*`, `game/engine_api.h`, `engine/obj/ui_text_oclass.h`

Purpose
- Central documentation for the UI subsystem: the GuiLayer, `ui.text` object, programmatic UI APIs, and scene serialization for UI elements.

Overview
- The GuiLayer provides screen-space rendering functionality independent of the world-space rendering pipeline. It handles programmatic UI entries (transient or persistent), renders `ui.text` object instances from the object registry, and manages font rasterization and glyph atlases.

Key concepts
- Programmatic entries: quick transient or persistent pieces of text you can add at runtime via `UIAddTextAtNDC()`.
- Object-backed UI: persistent UI elements created as `ui.text` objects (via scene files or `InstantiateUIText()`), which can be modified at runtime and serialized when saving scenes.
- Font handling: GuiLayer discovers fonts under `game/demo/fonts` by default (engine assumes the game runs with CWD=`game`). You may also supply an explicit font path when creating UI text.

API & usage

- UIAddTextAtNDC
  - Signature: `void UIAddTextAtNDC(const std::string& text, float ndc_x, float ndc_y, const std::string& font = "", int pxSize = 24, bool persistent = false)`
  - Adds text at NDC coordinates (range -1..1). If `persistent` is true the entry remains until removed; otherwise it's transient for the current frame.
  - Example: `UIAddTextAtNDC("Lives: 3", -0.9f, 0.9f, "demo/fonts/DMSans.ttf", 20);`

- InstantiateUIText
  - Signature: `Object* InstantiateUIText(const std::string& text, float ndc_x, float ndc_y, const std::string& font = "", int size = 24, const std::string& instName = "")`
  - Creates an object-backed `ui.text` instance registered in the engine's object manager. Returns the created object for later manipulation. If `instName` is empty a unique `ui_text_<n>` name is generated.
  - Example:
    ```cpp
    Object* label = InstantiateUIText("Score: 0", -0.9f, 0.8f, "demo/fonts/DMSans.ttf", 24, "score_label");
    UIText_OBJ* ul = dynamic_cast<UIText_OBJ*>(label);
    if (ul) ul->text = "Score: 1";
    ```

`ui.text` object (properties)
- Class: `UIText_OBJ` (registered as `ui.text`)
- Common properties supported in prototypes and scene `[...]` blocks:
  - `text` (string)
  - `font` (string) — path to a TTF file or empty to use the default discovered font
  - `size` (int) — pixel font size
  - `nx`, `ny` (float) — normalized coordinates (0..1) that map to NDC for placement. If provided, they are preferred over `sx`/`sy`.
  - `sx`, `sy` (float) — pixel coordinates (screen space)
  - `r`, `g`, `b`, `a` (float) — color components (0..1)

Notes & details
- `ui.*` objects do not participate in standard engine texture resolution; GuiLayer handles rendering via glyph atlases.
- Fonts are cached to avoid repeated loading; glyphs are generated on demand and added to the atlas.
- Coordinates: the scene `UI` syntax expects NDC (-1..1), but the object properties `nx`/`ny` stored as 0..1 internally for convenience.

Scene serialization & UI syntax
- Use the `UI` operation in `.fscn` files to place UI objects at NDC coordinates and provide properties in square brackets:

```
UI label ui.text 0.05 0.95
[
    text "Hello, world!";
    font "demo/fonts/DMSans.ttf";
    size 24;
];
```

- `sceneManager::saveScene()` will write `ui.text` objects using this form (properties in `[...]`). Children (if any) remain written in a following `{ ... }` block.

Examples
- Transient programmatic:
```
UIAddTextAtNDC("Hello", 0.0f, 0.0f);
```
- Persistent object-backed (scene):
```
UI score_label ui.text -0.9 0.9
[
    text "Score: 0";
    size 20;
];
```

Troubleshooting
- If text does not appear, verify the font file path (relative to `game/`), and ensure glyphs for the characters are rasterized (they are added on-demand when first used).
- For layout issues, confirm whether `nx/ny` (preferred) or `sx/sy` are being used; `nx/ny` are normalized, `sx/sy` are pixels.

