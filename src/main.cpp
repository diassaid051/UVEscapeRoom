#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "HUD.h"
#include <iostream>
#include <cstdio>
#include <filesystem>
#include <string>

static Camera camera;
static Scene scene;
static HUD hud;
static bool showHelp = true;
static bool firstMouse = true;
static double lastX = 0, lastY = 0;
static int fbW = 1280, fbH = 720;

static void resetGame() {                       // back to the start of the game
    scene.reset();
    camera.position = glm::vec3(0.0f, 1.6f, 2.5f);
    camera.yaw = -90.0f; camera.pitch = 0.0f;
}
static std::string formatTime(float t) {        // mm:ss
    char buf[16]; int s = (int)t;
    std::snprintf(buf, sizeof(buf), "%02d:%02d", s / 60, s % 60);
    return buf;
}

static void onMouse(GLFWwindow*, double x, double y) {
    if (firstMouse) { lastX = x; lastY = y; firstMouse = false; }
    if (!scene.keypadOpen() && !scene.finished()) camera.rotate((float)(x - lastX), (float)(y - lastY));
    lastX = x; lastY = y;
}
static void onResize(GLFWwindow*, int w, int h) { fbW = w; fbH = h; glViewport(0, 0, w, h); }
static void onKey(GLFWwindow* win, int key, int, int action, int) {
    if (action != GLFW_PRESS) return;
    if (scene.keypadOpen()) {                      // while the code panel is open, keys go to the lock
        if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)             scene.keypadDigit((char)('0' + key - GLFW_KEY_0));
        else if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9)  scene.keypadDigit((char)('0' + key - GLFW_KEY_KP_0));
        else if (key == GLFW_KEY_BACKSPACE)                      scene.keypadBackspace();
        else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) scene.keypadConfirm();
        else if (key == GLFW_KEY_ESCAPE)                         scene.keypadCancel();
        return;
    }
    if (scene.finished()) {                        // win screen: only R (restart) and Esc (quit)
        if (key == GLFW_KEY_R) resetGame();
        else if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(win, true);
        return;
    }
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(win, true);
    if (key == GLFW_KEY_F) scene.flashOn = !scene.flashOn;
    if (key == GLFW_KEY_L) scene.lampOn = !scene.lampOn;
    if (key == GLFW_KEY_E) scene.interact(camera.position);
    if (key == GLFW_KEY_H) showHelp = !showHelp;
}

int main(int, char** argv) {
    // Make relative paths (shaders/, assets/) work no matter where the program is started from.
    std::error_code ec;
    std::filesystem::current_path(std::filesystem::absolute(argv[0]).parent_path(), ec);
    if (!std::filesystem::exists("shaders/scene.vert")) {
        std::cerr << "ERROR: shaders/ folder not found next to the executable ("
            << std::filesystem::current_path().string() << ")\n";
        return -1;
    }
    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* win = glfwCreateWindow(1280, 720, "UV Escape Room", nullptr, nullptr);
    if (!win) { std::cerr << "Window creation failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) { std::cerr << "GLAD failed\n"; return -1; }

    glfwSetFramebufferSizeCallback(win, onResize);
    glfwSetCursorPosCallback(win, onMouse);
    glfwSetKeyCallback(win, onKey);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetFramebufferSize(win, &fbW, &fbH); glViewport(0, 0, fbW, fbH);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);                        // all faces are CCW outward
    glClearColor(0.01f, 0.01f, 0.02f, 1.0f);

    Shader shader("shaders/scene.vert", "shaders/scene.frag");
    scene.init();
    hud.init();
    camera.position = glm::vec3(0.0f, 1.6f, 2.5f);

    double last = glfwGetTime(), fpsTimer = last; int frames = 0; int fps = 0;
    while (!glfwWindowShouldClose(win)) {
        double now = glfwGetTime(); float dt = (float)(now - last); last = now;
        if (dt > 0.05f) dt = 0.05f;            // a long stall must not teleport the player through walls
        glfwPollEvents();

        float fwd = 0, right = 0;
        if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) fwd += 1;
        if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) fwd -= 1;
        if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) right += 1;
        if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) right -= 1;
        bool sprint = glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        if (!scene.keypadOpen() && !scene.finished()) scene.move(camera.position, camera.moveDelta(fwd, right, dt, sprint));
        scene.update(dt);
        scene.checkWin(camera.position);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene.draw(shader, camera, fbH > 0 ? (float)fbW / fbH : 1.0f);

        // ---- 2D overlay ----
        hud.begin(fbW, fbH);
        hud.text("FPS: " + std::to_string(fps), 12, 10, 2.0f, glm::vec3(0.4f, 1.0f, 0.5f));
        {   // game timer, top-right
            std::string t = "TIME " + formatTime(scene.playTime());
            hud.text(t, fbW - hud.textWidth(t, 2.5f) - 14, 10, 2.5f, glm::vec3(1.0f));
        }
        if (showHelp) {
            const glm::vec3 c(0.9f, 0.9f, 0.95f);
            hud.text("WASD - MOVE   SHIFT - SPRINT   MOUSE - LOOK", 12, 36, 2.0f, c);
            hud.text("F - UV LIGHT   L - LAMP   E - INTERACT   H - HIDE HELP", 12, 56, 2.0f, c);
        }
        hud.text("+", fbW * 0.5f - 7, fbH * 0.5f - 7, 2.0f, glm::vec3(1.0f), 0.7f);   // crosshair
        hud.textCentered(scene.hint(camera.position), fbH - 40.0f, 2.5f, glm::vec3(1.0f, 0.9f, 0.5f));

        if (scene.keypadOpen()) {                  // code lock panel
            hud.rect(0, 0, (float)fbW, (float)fbH, glm::vec4(0, 0, 0, 0.6f));
            float pw = 640, ph = 300, px = (fbW - pw) * 0.5f, py = (fbH - ph) * 0.5f;
            hud.rect(px - 4, py - 4, pw + 8, ph + 8, glm::vec4(0.55f, 0.45f, 1.0f, 1.0f));
            hud.rect(px, py, pw, ph, glm::vec4(0.06f, 0.06f, 0.12f, 1.0f));
            hud.textCentered("ENTER 4-DIGIT CODE", py + 24, 3.5f, glm::vec3(0.8f, 0.75f, 1.0f));
            std::string shown;
            for (size_t i = 0; i < 4; ++i) { shown += (i < scene.keypadInput().size() ? scene.keypadInput()[i] : '_'); shown += ' '; }
            hud.textCentered(shown, py + 100, 9.0f, glm::vec3(0.4f, 1.0f, 0.75f));
            if (scene.keypadWrong()) hud.textCentered("WRONG CODE", py + 185, 4.0f, glm::vec3(1.0f, 0.3f, 0.3f));
            hud.textCentered("0-9 TYPE   BACKSPACE DELETE   ENTER OK   ESC CANCEL", py + 250, 2.0f, glm::vec3(0.8f));
        }

        if (scene.finished()) {                    // win screen
            hud.rect(0, 0, (float)fbW, (float)fbH, glm::vec4(0.02f, 0.0f, 0.08f, 0.82f));
            hud.textCentered("YOU ESCAPED!", fbH * 0.5f - 130, 12.0f, glm::vec3(0.4f, 1.0f, 0.75f));
            hud.textCentered("TIME: " + formatTime(scene.playTime()), fbH * 0.5f + 10, 5.0f, glm::vec3(1.0f));
            hud.textCentered("THE UV LIGHT REVEALED THE CODE. WELL DONE, DETECTIVE.", fbH * 0.5f + 90, 2.5f, glm::vec3(0.8f, 0.75f, 1.0f));
            hud.textCentered("[R] PLAY AGAIN     [ESC] QUIT", fbH * 0.5f + 150, 3.0f, glm::vec3(1.0f, 0.9f, 0.5f));
        }
        hud.end();
        glfwSwapBuffers(win);

        ++frames;
        if (now - fpsTimer >= 0.5) {                // FPS counter in the window title (R7)
            fps = (int)(frames / (now - fpsTimer)); frames = 0; fpsTimer = now;
            char title[256];
            std::snprintf(title, sizeof(title), "UV Escape Room  |  FPS: %d", fps);
            glfwSetWindowTitle(win, title);
        }
    }
    glfwTerminate();
    return 0;
}
