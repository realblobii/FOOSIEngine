# Render pipeline

Location: `engine/render/*`

Purpose
- Describe the rendering pipeline and the GuiLayer which handles screen-space UI rendering.

Overview
- The render pipeline collects textures and objects and builds a texture atlas for batched draws.
- `renderPipeline::renderAll()` draws the current scene using the atlas and per-object transforms.

Usage & GuiLayer
- The GuiLayer handles screen-space UI rendering (text labels, programmatic UI entries) and uses FreeType to rasterize font glyphs into a glyph atlas.
- Fonts are discovered starting in the working `game/` directory under `demo/fonts` (the engine assumes the game is run with CWD=`game`). You can specify a font path explicitly (e.g., `demo/fonts/DMSans.ttf`) when creating `ui.text` objects or calling `UIAddTextAtNDC()`.
- The GuiLayer caches font handles and only loads fonts/glyphs once; glyphs are added to the atlas only when first needed to minimize texture churn and CPU work.

Notes
- The renderer is OpenGL 3.3 core-profile oriented (GLAD + SDL_GL context created in `Engine::Init`).
- Textures are loaded via `Texture` helpers and combined into an atlas. If you add textures, ensure their lifetime is managed by the pipeline.
- The rendering subsystem is lower-level; prefer high-level helpers in game code when available.

## GuiLayer & fonts

- The GuiLayer handles screen-space UI rendering (text labels, programmatic UI entries) and uses FreeType to rasterize font glyphs into a glyph atlas.
- Fonts are discovered starting in the working `game/` directory under `demo/fonts` (the engine assumes the game is run with CWD=`game`). You can specify a font path explicitly (e.g., `demo/fonts/DMSans.ttf`) when creating `ui.text` objects or calling `UIAddTextAtNDC()`.
- The GuiLayer caches font handles and only loads fonts/glyphs once; glyphs are added to the atlas only when first needed to minimize texture churn and CPU work.
