#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Compiles and links a vertex + fragment shader loaded from files.
class Shader {
public:
    GLuint id = 0;

    Shader(const std::string& vertPath, const std::string& fragPath) {
        GLuint v = compile(GL_VERTEX_SHADER, readFile(vertPath), vertPath);
        GLuint f = compile(GL_FRAGMENT_SHADER, readFile(fragPath), fragPath);
        id = glCreateProgram();
        glAttachShader(id, v);
        glAttachShader(id, f);
        glLinkProgram(id);
        GLint ok; glGetProgramiv(id, GL_LINK_STATUS, &ok);
        if (!ok) { char log[1024]; glGetProgramInfoLog(id, 1024, nullptr, log); std::cerr << "Link error:\n" << log << "\n"; }
        glDeleteShader(v); glDeleteShader(f);
    }
    void use() const { glUseProgram(id); }

    void setInt(const char* n, int v)             const { glUniform1i(loc(n), v); }
    void setBool(const char* n, bool v)           const { glUniform1i(loc(n), (int)v); }
    void setFloat(const char* n, float v)         const { glUniform1f(loc(n), v); }
    void setVec2(const char* n, const glm::vec2& v) const { glUniform2fv(loc(n), 1, glm::value_ptr(v)); }
    void setVec4(const char* n, const glm::vec4& v) const { glUniform4fv(loc(n), 1, glm::value_ptr(v)); }
    void setVec3(const char* n, const glm::vec3& v) const { glUniform3fv(loc(n), 1, glm::value_ptr(v)); }
    void setMat4(const char* n, const glm::mat4& m) const { glUniformMatrix4fv(loc(n), 1, GL_FALSE, glm::value_ptr(m)); }

private:
    GLint loc(const char* n) const { return glGetUniformLocation(id, n); }
    static std::string readFile(const std::string& path) {
        std::ifstream f(path);
        if (!f) std::cerr << "Cannot open shader file: " << path << "\n";
        std::stringstream ss; ss << f.rdbuf(); return ss.str();
    }
    static GLuint compile(GLenum type, const std::string& src, const std::string& name) {
        GLuint s = glCreateShader(type);
        const char* c = src.c_str();
        glShaderSource(s, 1, &c, nullptr);
        glCompileShader(s);
        GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) { char log[1024]; glGetShaderInfoLog(s, 1024, nullptr, log); std::cerr << "Compile error in " << name << ":\n" << log << "\n"; }
        return s;
    }
};
