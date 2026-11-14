#pragma once
#include <glad/gl.h>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>

// All textures are generated procedurally (no external files needed), uploaded
// with mipmaps and GL_REPEAT. Use loadFile() to swap in real images later.
namespace Texture {

inline GLuint upload(int w, int h, const std::vector<uint8_t>& rgba) {
    GLuint t; glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return t;
}

inline float hash(int x, int y) {
    unsigned n = (unsigned)(x * 374761393 + y * 668265263);
    n = (n ^ (n >> 13)) * 1274126177u;
    return ((n ^ (n >> 16)) & 0xFFFF) / 65535.0f;
}
inline void put(std::vector<uint8_t>& p, int w, int x, int y, float r, float g, float b) {
    auto c = [](float v) { return (uint8_t)(std::fmin(std::fmax(v, 0.f), 1.f) * 255); };
    size_t i = ((size_t)y * w + x) * 4; p[i] = c(r); p[i+1] = c(g); p[i+2] = c(b); p[i+3] = 255;
}

// Wood: stripes + grain noise
inline GLuint wood() {
    const int W = 256, H = 256; std::vector<uint8_t> p(W * H * 4);
    for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) {
        float plank = std::fmod(x / 64.0f, 1.0f);
        float grain = 0.5f + 0.5f * std::sin(y * 0.09f + hash(x / 64, 0) * 20.0f + hash(x, y / 8) * 1.5f);
        float edge  = (plank < 0.02f) ? 0.5f : 1.0f;
        float v = (0.35f + 0.25f * grain) * edge;
        put(p, W, x, y, v * 1.15f, v * 0.75f, v * 0.45f);
    }
    return upload(W, H, p);
}
// Concrete / plaster wall
inline GLuint wall() {
    const int W = 256, H = 256; std::vector<uint8_t> p(W * H * 4);
    for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) {
        float n = 0.6f * hash(x / 4, y / 4) + 0.4f * hash(x, y);
        float v = 0.45f + 0.25f * n;
        put(p, W, x, y, v, v * 0.98f, v * 0.95f);
    }
    return upload(W, H, p);
}
// Floor tiles (checker with grout)
inline GLuint floorTiles() {
    const int W = 256, H = 256; std::vector<uint8_t> p(W * H * 4);
    for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) {
        bool grout = (x % 64 < 2) || (y % 64 < 2);
        bool dark  = ((x / 64) + (y / 64)) % 2 == 0;
        float v = grout ? 0.12f : (dark ? 0.30f : 0.38f);
        v += 0.05f * hash(x, y);
        put(p, W, x, y, v, v, v * 1.05f);
    }
    return upload(W, H, p);
}

// Hidden UV message rendered from a tiny built-in 5x7 font. White text on black.
inline GLuint hiddenMessage(const std::string& text) {
    struct G { char c; const char* rows[7]; };
    static const G font[] = {
        {'C', {"01111","10000","10000","10000","10000","10000","01111"}},
        {'O', {"01110","10001","10001","10001","10001","10001","01110"}},
        {'D', {"11110","10001","10001","10001","10001","10001","11110"}},
        {'E', {"11111","10000","10000","11110","10000","10000","11111"}},
        {'T', {"11111","00100","00100","00100","00100","00100","00100"}},
        {'H', {"10001","10001","10001","11111","10001","10001","10001"}},
        {'4', {"00010","00110","01010","10010","11111","00010","00010"}},
        {'2', {"01110","10001","00001","00010","00100","01000","11111"}},
        {'7', {"11111","00001","00010","00100","01000","01000","01000"}},
        {'1', {"00100","01100","00100","00100","00100","00100","01110"}},
    };
    const int W = 512, H = 128, S = 9;           // S = pixels per font dot
    std::vector<uint8_t> p(W * H * 4, 0);
    for (size_t i = 0; i < p.size(); i += 4) p[i + 3] = 255;
    int totalW = (int)text.size() * 6 * S;
    int ox = (W - totalW) / 2, oy = (H - 7 * S) / 2;
    for (size_t k = 0; k < text.size(); ++k) {
        for (const G& g : font) if (g.c == text[k])
            for (int r = 0; r < 7; ++r) for (int c = 0; c < 5; ++c) if (g.rows[r][c] == '1')
                for (int dy = 0; dy < S; ++dy) for (int dx = 0; dx < S; ++dx) {
                    int x = ox + (int)k * 6 * S + c * S + dx;
                    int y = H - 1 - (oy + r * S + dy);          // flip: row 0 of the image is the bottom in GL
                    put(p, W, x, y, 1, 1, 1);
                }
    }
    return upload(W, H, p);
}

} // namespace Texture
