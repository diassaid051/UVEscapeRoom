# UV Escape Room (Computer Graphics Fundamentals - Final Project)

A detective's office where the clue is only visible under an ultraviolet flashlight.
C++17, OpenGL 3.3 core profile, GLFW + GLAD + GLM (all included in `external/`).

## Build (Windows, Visual Studio 2022)
1. Install Visual Studio 2022 with the "Desktop development with C++" workload (includes CMake).
2. `File -> Open -> Folder...` and select this folder. Wait for "CMake generation finished".
3. Select `UVEscapeRoom.exe` as the startup item and the `x64-Release` configuration, then press Start.

Command line alternative:
```
cmake -B build -S .
cmake --build build --config Release
build\Release\UVEscapeRoom.exe
```
Shaders and assets are copied next to the exe automatically.

## Controls
| Key | Action |
|---|---|
| W A S D | Move (Shift = sprint) |
| Mouse | Look |
| F | UV flashlight on/off |
| L | Desk lamp on/off |
| E | Interact (drawer, key, door) |
| Esc | Quit |

## How to play
Sweep the UV light over the grey board on the north wall. Open the desk drawer (E), take the key (E again),
unlock the east door (E) and walk out. The FPS counter is in the window title.

## Features (requirements)
- R1 core profile, VAO/VBO/EBO, own shaders in `shaders/`
- R2 room shell, desk, drawer, lamp, cabinet, crates, door, board, key
- R3 own MVP matrices; hierarchy: desk -> drawer/lamp, hinge -> door/handle
- R4 per-fragment Blinn-Phong; two lights: desk lamp (point, toggle) + UV flashlight (spot)
- R5 mipmapped procedural textures (wood, wall, tiles) + hidden-writing texture
- R6 FPS camera + 4 interactions (flashlight, lamp, drawer/key, door)
- R7 FPS in window title

## Team and roles
- (name, group) - everything (solo project, approved by the instructor)

## AI Use
Claude (Anthropic) was used to generate the initial project skeleton and shaders. (Describe what you changed.)
