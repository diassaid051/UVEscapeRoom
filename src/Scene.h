#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

// The escape room: geometry, hierarchy (desk -> drawer/lamp, hinge -> door),
// game state and collision.
class Scene {
public:
    void init();
    void update(float dt);
    void draw(const Shader& sh, const Camera& cam, float aspect) const;

    // collision-aware movement of the player on the floor plane
    void move(glm::vec3& pos, const glm::vec3& delta) const;
    void interact(const glm::vec3& playerPos);
    std::string hint(const glm::vec3& playerPos) const;   // hint text shown on screen
    bool escaped(const glm::vec3& p) const { return p.x > 4.4f; }

    // ---- win screen / timer ----
    void checkWin(const glm::vec3& playerPos);   // call every frame: latches the win when the player leaves the room
    bool finished() const { return finished_; }
    float playTime() const { return finished_ ? finalTime_ : playTime_; }
    void reset();                                // restart the game (lamp/flashlight settings are kept)

    // ---- code lock on the door ----
    bool keypadOpen() const { return keypadOpen_; }
    const std::string& keypadInput() const { return input_; }
    bool keypadWrong() const { return wrongTimer_ > 0.0f; }
    void keypadDigit(char c);
    void keypadBackspace();
    void keypadConfirm();
    void keypadCancel();

    bool lampOn = true, flashOn = true;

private:
    struct Box { glm::vec3 min, max; };
    struct Item {                       // one drawable object
        glm::mat4 model; glm::vec3 color; GLuint tex; glm::vec2 uv;   // uv: legacy, ignored (tiling now follows the world size)
        float shininess; bool hidden = false, emissive = false;
    };
    void drawItem(const Shader& sh, const Item& it) const;
    bool walkable(const glm::vec3& p) const;

    Mesh cube_;
    GLuint woodT_ = 0, wallT_ = 0, floorT_ = 0, hiddenT_ = 0;
    std::vector<Box> blockers_;

    // state
    bool drawerOpen_ = false, keyTaken_ = false, doorOpen_ = false;
    bool keypadOpen_ = false, codeSolved_ = false;
    bool finished_ = false;
    float playTime_ = 0.0f, finalTime_ = 0.0f;
    std::string input_;
    float wrongTimer_ = 0.0f;
    const std::string correctCode_ = "4271";   // the same digits are written in the hidden UV message
    float drawerT_ = 0.0f, doorT_ = 0.0f;       // animation progress 0..1
    std::string message_; float messageTimer_ = 0.0f;

    const glm::vec3 deskPos_{ -2.2f, 0.4f, -3.2f };
    const glm::vec3 lampPos_{ -2.9f, 1.15f, -3.2f };
    void say(const std::string& s) { message_ = s; messageTimer_ = 4.0f; }
};
