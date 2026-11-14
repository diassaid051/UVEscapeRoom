#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// First-person camera. Movement is multiplied by delta time (frame-rate independent).
class Camera {
public:
    glm::vec3 position{0.0f, 1.6f, 3.0f};
    float yaw = -90.0f, pitch = 0.0f;
    float speed = 2.6f, sensitivity = 0.1f, fov = 70.0f;

    glm::vec3 front() const {
        float y = glm::radians(yaw), p = glm::radians(pitch);
        return glm::normalize(glm::vec3(cos(y) * cos(p), sin(p), sin(y) * cos(p)));
    }
    glm::mat4 view() const { return glm::lookAt(position, position + front(), glm::vec3(0, 1, 0)); }
    glm::mat4 projection(float aspect) const { return glm::perspective(glm::radians(fov), aspect, 0.05f, 50.0f); }

    void rotate(float dx, float dy) {
        yaw += dx * sensitivity;
        pitch = glm::clamp(pitch - dy * sensitivity, -89.0f, 89.0f);
    }
    // returns the desired displacement on the floor plane; the caller does collision
    glm::vec3 moveDelta(float forward, float right, float dt, bool sprint) const {
        glm::vec3 f = front(); f.y = 0; f = glm::normalize(f);
        glm::vec3 r = glm::normalize(glm::cross(f, glm::vec3(0, 1, 0)));
        glm::vec3 d = f * forward + r * right;
        if (glm::length(d) > 0.0f) d = glm::normalize(d);
        return d * speed * (sprint ? 1.8f : 1.0f) * dt;
    }
};
