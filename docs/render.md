# Render pipeline

Location: `engine/render/*`

Overview
- The render pipeline collects textures and objects and builds a texture atlas for batched draws.
- `renderPipeline::renderAll()` draws the current scene using the atlas and per-object transforms.

Notes
- The renderer is OpenGL 3.3 core-profile oriented (GLAD + SDL_GL context created in `Engine::Init`).
- Textures are loaded via `Texture` helpers and combined into an atlas. If you add textures, ensure their lifetime is managed by the pipeline.
- The rendering subsystem is lower-level; prefer high-level helpers in game code when available.
