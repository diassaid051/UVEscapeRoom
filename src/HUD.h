#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>
#include "Shader.h"

// Screen-space text drawn with a built-in 5x7 bitmap font (no external font files).
class HUD {
public:
    void init();
    void begin(int screenW, int screenH);    // set up 2D state (no depth, no culling, blending)
    void end();                              // restore 3D state
    // x, y in pixels from the top-left corner; scale = size of one font dot in pixels
    void text(const std::string& s, float x, float y, float scale, const glm::vec3& color, float alpha = 1.0f);
    void textCentered(const std::string& s, float y, float scale, const glm::vec3& color);
    float textWidth(const std::string& s, float scale) const { return s.size() * 6.0f * scale; }

private:
    void drawString(const std::string& s, float x, float y, float scale, const glm::vec4& color);
    Shader* shader_ = nullptr;
    GLuint vao_ = 0, vbo_ = 0, atlas_ = 0;
    int w_ = 1, h_ = 1;
};
