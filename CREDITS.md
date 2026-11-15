# Credits

## Libraries (all included in `external/`)
| Library | Version | Author | License | Link |
|---|---|---|---|---|
| GLFW | 3.4 | Marcus Geelnard, Camilla Löwy and contributors | zlib/libpng | https://www.glfw.org |
| GLM | 1.0.1 | G-Truc Creation | MIT (Happy Bunny) | https://github.com/g-truc/glm |
| GLAD 2 (loader generator) | 2.0.8, generated for gl:core 3.3 | David Herberth | MIT (generator); generated code follows the Khronos / public-domain terms | https://github.com/Dav1dde/glad |
| stb_image | latest at time of download | Sean Barrett | Public domain / MIT | https://github.com/nothings/stb |

## Tutorials and references
- LearnOpenGL (Joey de Vries), CC BY 4.0, https://learnopengl.com - structure of the Blinn-Phong lighting and spotlight (soft edge with inner/outer cut-off), cited in `shaders/scene.frag`.

## Assets
- No third-party models, textures, sounds or fonts are used.
- All textures (wood, plaster, floor tiles, hidden message) are generated in code (`src/Texture.h`).
- The 5x7 bitmap font used for the hidden message and the HUD is hand-written in `src/Texture.h` and `src/HUD.cpp`.
- All 3D models are built from one cube mesh.

## AI tools
- Claude (Anthropic) - code generation and troubleshooting help. Details: "AI Use" section of `README.md` and of the report.
