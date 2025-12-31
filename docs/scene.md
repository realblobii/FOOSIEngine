# Scene file format & Scene manager

Location: `engine/scene/*` and the configured scene folder (default: `game/demo/scn`)

This page documents the textual scene format and the `sceneManager` API used to load and save scenes.

## File layout

A scene file uses a simple line-oriented format with a header and footer:

- Header: `#SCNDEF <scene-name>` where `<scene-name>` is a short identifier (no `.fscn` suffix required in the header)
- Body: one or more `OBJECT` and `SCENE` statements and optional nested blocks
- Footer: `#ENDSCN`

Example (short):

```
#SCNDEF test
{
    OBJECT tile.grass 10 10 0;
    OBJECT tile.pTest 10 10 1
    {
        OBJECT tile.grass 0 0 1;
    };
    SCENE test2.fscn 11 11 0;
};
#ENDSCN
```

## Syntax rules

- Lines are trimmed and empty lines ignored.
- Commands use an initial keyword and arguments separated by whitespace.
- Commands include:
  - `OBJECT <name> <class>[.<subclass>] <x> <y> <z>` — instantiate an object with the specified instance `name` and prototype `class`/`subclass` at the given coordinates.
    - If the line ends with `;` it is a leaf object. If the line is followed by a `{` block, the following objects become children of this object (see *Child blocks* below). Note: the parser expects the `name` token first, then the `class.subclass` token.
  - `SCENE <path> <x> <y> <z>` — create a reference to another scene file. The `path` should be a valid scene filename (e.g., `test2.fscn`). When loading, `SCENE` will load the referenced scene and attach its scene object as a child.

### Child blocks

- You can nest objects by placing a `{` on its own line after an `OBJECT` line, then placing child `OBJECT` or `SCENE` lines inside the block, and terminate the block with `};` (the close brace optionally followed by `;`).
- If an `OBJECT` is not followed by a `{` it is treated as a leaf and must end with `;`.
- Example multi-level nesting:

```
OBJECT tile.parent 10 10 0
{
    OBJECT tile.child 1 1 1;
    OBJECT tile.withgrand 2 2 1
    {
        OBJECT tile.grandchild 0 1 0;
    };
};
```

- If a closing brace includes a trailing `;` (i.e., `};`) it not only closes the block, but terminates the *most recent* object so subsequent sibling blocks are not nested under it.

## Loading behavior

- `sceneManager::loadScene(path, baseX, baseY, baseZ)` reads the specified file from the configured scene folder and instantiates objects.
- The loader maintains a stack of parent objects to support nested `{}` blocks; if no previous object exists for a `{` it treats the scene root as the parent for that block.
- Nested `SCENE` references are loaded recursively; the nested scene's root object is attached to the parent, and all object IDs from the nested load are aggregated into the calling scene's `sceneData`.
- All instantiated coordinates are the sum of the explicit coordinates and the `baseX`, `baseY`, `baseZ` offsets passed to `loadScene`.
### New UI syntax (`UI`)

- `UI` is a new operation specifically for UI objects (e.g., `ui.text`) that are positioned in normalized device coordinates (NDC) instead of world-space x/y.
- Syntax: `UI <name> <class>[.<subclass>] <nx> <ny> [ <properties> ];` or the multi-line form:

```
UI my_label ui.text 0.05 0.95
[
    text "Hello, world!";
    font "demo/fonts/DMSans.ttf";
    size 24;
];
```

- The `<nx>` and `<ny>` are NDC coordinates in the range [-1..1]; for convenience the engine stores them internally as normalized [0..1] (i.e., `(ndc+1)/2`).
- Properties for `UI` objects are specified inside square brackets `[...]` and applied to the instantiated object with the same property parsing rules as object `{ ... }` blocks (see *Properties* below).
- Children (if any) remain specified with curly `{ ... }` blocks following the UI block.

## Saving behavior

- `sceneManager::saveScene(name)` serializes a scene back to a `.fscn` file in the scene folder.
- Behavior details:
  - If the scene folder does not exist it is created automatically.
  - The header `#SCNDEF <name>` uses the scene's name without a `.fscn` suffix.
  - Coordinates are written as integers relative to the saved scene's root object.
  - Nested `Scene_OBJ` instances (child scenes) are written as `SCENE <scene-name>.fscn <x> <y> <z>;` and their children are intentionally not expanded in the file (a child scene is saved as a reference only).
  - `ui.text` objects and other UI objects are saved using the new `UI` syntax. Their visible properties (text, font, size, nx/ny) are written inside a square-bracket property block `[...]`. If the object has children, they are written in a following `{ ... }` block (children remain curly-braced).
  - Other objects continue to use the existing `OBJECT` / `{ ... }` syntax for children.

## Notes & constraints

- The engine expects object prototypes (class+subclass) to be defined in `game/assets/objects.json`. `Instantiate()` will fail if a prototype is missing.
- The parser is forgiving about spacing but requires `;` terminators (or an inline `{` block) to disambiguate object termination.

## Example files

- See `game/demo/scn/test.fscn` and `game/demo/scn/test_child.fscn` for working examples of nested child syntax and `SCENE` references.

If you want `saveScene` to also persist per-object properties (textures, custom properties), I can extend the file format to include a mini-property block for objects; tell me your preferred syntax and I'll implement it.
