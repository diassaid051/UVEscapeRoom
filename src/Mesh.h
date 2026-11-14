#pragma once
#include <glad/gl.h>
#include <vector>

// A unit cube (1x1x1, centred at the origin) with normals and UVs.
// Every object in the scene is this cube scaled/rotated/translated.
class Mesh {
public:
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLsizei indexCount = 0;

    static Mesh cube() {
        // position, normal, uv   (4 vertices per face -> correct flat normals)
        struct Face { float n[3]; float p[4][3]; };
        const Face faces[6] = {
            {{ 0, 0, 1}, {{-.5f,-.5f, .5f},{ .5f,-.5f, .5f},{ .5f, .5f, .5f},{-.5f, .5f, .5f}}},
            {{ 0, 0,-1}, {{ .5f,-.5f,-.5f},{-.5f,-.5f,-.5f},{-.5f, .5f,-.5f},{ .5f, .5f,-.5f}}},
            {{ 1, 0, 0}, {{ .5f,-.5f, .5f},{ .5f,-.5f,-.5f},{ .5f, .5f,-.5f},{ .5f, .5f, .5f}}},
            {{-1, 0, 0}, {{-.5f,-.5f,-.5f},{-.5f,-.5f, .5f},{-.5f, .5f, .5f},{-.5f, .5f,-.5f}}},
            {{ 0, 1, 0}, {{-.5f, .5f, .5f},{ .5f, .5f, .5f},{ .5f, .5f,-.5f},{-.5f, .5f,-.5f}}},
            {{ 0,-1, 0}, {{-.5f,-.5f,-.5f},{ .5f,-.5f,-.5f},{ .5f,-.5f, .5f},{-.5f,-.5f, .5f}}},
        };
        const float uv[4][2] = {{0,0},{1,0},{1,1},{0,1}};
        std::vector<float> v; std::vector<unsigned> idx;
        for (int f = 0; f < 6; ++f) {
            for (int i = 0; i < 4; ++i) {
                v.insert(v.end(), faces[f].p[i], faces[f].p[i] + 3);
                v.insert(v.end(), faces[f].n, faces[f].n + 3);
                v.insert(v.end(), uv[i], uv[i] + 2);
            }
            unsigned b = f * 4;
            idx.insert(idx.end(), {b, b+1, b+2, b, b+2, b+3});
        }
        Mesh m; m.indexCount = (GLsizei)idx.size();
        glGenVertexArrays(1, &m.vao); glGenBuffers(1, &m.vbo); glGenBuffers(1, &m.ebo);
        glBindVertexArray(m.vao);
        glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
        glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(unsigned), idx.data(), GL_STATIC_DRAW);
        GLsizei stride = 8 * sizeof(float);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);                   glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
        glBindVertexArray(0);
        return m;
    }
    void draw() const { glBindVertexArray(vao); glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0); }
};
