#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include <iostream>
#include <cstdio>

static Camera camera;
static Scene scene;
static bool firstMouse = true;
static double lastX = 0, lastY = 0;
static int fbW = 1280, fbH = 720;

static void onMouse(GLFWwindow*, double x, double y) {
    if (firstMouse) { lastX = x; lastY = y; firstMouse = false; }
    camera.rotate((float)(x - lastX), (float)(y - lastY));
    lastX = x; lastY = y;
}
static void onResize(GLFWwindow*, int w, int h) { fbW = w; fbH = h; glViewport(0, 0, w, h); }
static void onKey(GLFWwindow* win, int key, int, int action, int) {
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(win, true);
    if (key == GLFW_KEY_F) scene.flashOn = !scene.flashOn;
    if (key == GLFW_KEY_L) scene.lampOn = !scene.lampOn;
    if (key == GLFW_KEY_E) scene.interact(camera.position);
}

int main() {
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
    camera.position = glm::vec3(0.0f, 1.6f, 2.5f);

    double last = glfwGetTime(), fpsTimer = last; int frames = 0; int fps = 0;
    while (!glfwWindowShouldClose(win)) {
        double now = glfwGetTime(); float dt = (float)(now - last); last = now;
        glfwPollEvents();

        float fwd = 0, right = 0;
        if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) fwd += 1;
        if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) fwd -= 1;
        if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) right += 1;
        if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) right -= 1;
        bool sprint = glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        scene.move(camera.position, camera.moveDelta(fwd, right, dt, sprint));
        scene.update(dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene.draw(shader, camera, fbH > 0 ? (float)fbW / fbH : 1.0f);
        glfwSwapBuffers(win);

        ++frames;
        if (now - fpsTimer >= 0.5) {                // FPS counter in the window title (R7)
            fps = (int)(frames / (now - fpsTimer)); frames = 0; fpsTimer = now;
            char title[256];
            if (scene.escaped(camera.position)) std::snprintf(title, sizeof(title), "YOU ESCAPED!  |  FPS: %d", fps);
            else std::snprintf(title, sizeof(title), "UV Escape Room  |  FPS: %d  |  %s", fps, scene.hint(camera.position).c_str());
            glfwSetWindowTitle(win, title);
        }
    }
    glfwTerminate();
    return 0;
}
