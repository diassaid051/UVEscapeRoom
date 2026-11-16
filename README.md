# UV Escape Room

Computer Graphics Fundamentals - Final Project (topic 3: *Escape Room with UV Light*).

You are a detective locked in an office. The clue to the exit is invisible to the naked eye:
it only glows inside the cone of your ultraviolet flashlight.

C++17, OpenGL 3.3 core profile. Everything needed to build (GLFW, GLAD, GLM, stb_image) is included in `external/`,
nothing has to be installed on the machine.

## How to play
1. Sweep the **UV flashlight** (F) over the grey board on the north wall: a 4-digit code glows into view.
2. Walk to the desk, open the drawer (**E**) and take the **key** (**E** again).
3. Go to the east door and press **E**: a code panel opens. Type the code and press Enter.
4. Walk out through the open door. A win screen shows your time; **R** restarts.

## Build

### Windows, Visual Studio 2022 (recommended)
1. Install Visual Studio 2022 with the **Desktop development with C++** workload (it includes CMake).
2. `File -> Open -> Folder...` and select the repository folder. Wait for "CMake generation finished".
3. Choose the **x64-Release** configuration, select `UVEscapeRoom.exe` as the startup item and press Start.

### Command line (any OS with CMake 3.15+ and a C++17 compiler)
```
cmake -B build -S .
cmake --build build --config Release
```
Run `build/Release/UVEscapeRoom.exe` (Windows) or `build/UVEscapeRoom` (Linux/macOS).
The `shaders/` and `assets/` folders are copied next to the executable automatically; the program
also switches its working directory to the executable's folder, so it can be started from anywhere.

Requirements: a GPU/driver with OpenGL 3.3 core profile. Tested on: NVIDIA GeForce RTX 3050, Windows 11.

## Controls
| Key | Action |
|---|---|
| W A S D | Move (hold Shift to sprint) |
| Mouse | Look around |
| F | UV flashlight on/off |
| L | Desk lamp on/off |
| E | Interact: drawer, key, door |
| 0-9, Backspace, Enter, Esc | Code panel: type, delete, confirm, cancel |
| V | V-Sync on/off (to measure the real FPS) |
| H | Show/hide the on-screen help |
| R | Restart (on the win screen) |
| Esc | Quit |

## Features and requirements
| Req. | Where it is in the project |
|---|---|
| R1 Modern pipeline | OpenGL 3.3 core, VAO/VBO/EBO (`src/Mesh.h`), own GLSL shaders in `shaders/`, no fixed-function calls |
| R2 Scene | Room shell, desk, drawer, desk lamp, cabinet, crates, door, UV board, key, rug, chair, bookshelf with books, painting (20+ objects built from one cube mesh) |
| R3 Transformations | Own model/view/projection matrices (GLM for math only). Hierarchy: desk -> drawer and lamp; hinge -> door and handle |
| R4 Lighting | Per-fragment Blinn-Phong (ambient + diffuse + specular), two lights: desk lamp (point, toggle L) and UV flashlight (spotlight from the camera, toggle F), with attenuation and soft cone edge |
| R5 Texturing | Procedural wood, plaster and floor textures with mipmaps and repeat wrapping; UVs are tiled by the real face size so nothing is stretched |
| R6 Interaction | FPS camera (mouse + keyboard); flashlight, lamp, drawer, key pickup, code lock, door |
| R7 Performance | FPS counter on screen (top-left) and in the window title |
| R8 Build | CMake build described above |

**The UV effect** (`shaders/scene.frag`): inside the flashlight cone the fragment shader adds the hidden-text texture
(`useHidden`) with a glow, weighted by `smoothstep` of the cone angle, so the writing fades in at the edge of the beam
and is completely invisible outside it.

### Stretch goals
1. **Code lock** - the door opens with a 4-digit code taken from the UV message, typed on an on-screen keypad.
2. **Win screen** - timer, win overlay with the final time, restart with R.

### Other
- On-screen HUD with its own bitmap font (`src/HUD.*`, `shaders/hud.*`): FPS, timer, help, hints, code panel.
- Frame-rate independent movement; collision with walls and furniture.

## Project structure
```
src/main.cpp      window, input, main loop, HUD drawing
src/Scene.*       room, hierarchy, game logic, collision
src/Shader.h      shader loading + uniform helpers
src/Mesh.h        cube mesh (VAO/VBO/EBO)
src/Camera.h      first-person camera
src/Texture.h     procedural textures + hidden message
src/HUD.*         2D text/rectangles overlay
shaders/          scene.vert/frag (3D), hud.vert/frag (2D)
external/         GLFW, GLAD, GLM, stb_image
```

## Performance
Measured in Release, 1280x720 (fill in after measuring):

| Hardware | Scene | Avg FPS |
|---|---|---|
| Intel core i5, NVIDIA GeForce RTX 3050, Windows 11 | Whole room, Release build | 144 (V-Sync, capped at the monitor refresh rate) |

## Team and roles
| Name | Group | Role |
|---|---|---|
| Ibrahim Nygyman | SE-2430 | Solo with AI |

## AI Use
AI tool used: **Claude (Anthropic)**.

What it was used for:
- Writing the first version of the project skeleton: CMake build, `Shader`, `Camera`, `Mesh`, `Texture`, `Scene`, `main.cpp` and the GLSL shaders (Blinn-Phong, spotlight, UV reveal effect).
- Writing the HUD module (bitmap font, text and rectangle drawing), the code lock, the win screen/timer, the room decoration and the quality pass (world-size UV tiling, uniform location cache, robust paths).
- Help with Visual Studio / CMake / Git troubleshooting.

What I did myself: set up the Visual Studio / CMake build and the Git/GitHub repository, integrated the code step by step, fixed build and repository problems, ran the project, tested the UV effect and every interaction (drawer, key, code lock, door, win screen, restart), and measured the FPS. [Add here anything else you changed yourself, e.g. positions, colours, texts.]


## License and credits
See [CREDITS.md](CREDITS.md).
