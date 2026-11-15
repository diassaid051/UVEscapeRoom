#include "Scene.h"
#include "Texture.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

using glm::vec3; using glm::vec2; using glm::mat4;

static mat4 TRS(vec3 pos, vec3 size) { return glm::scale(glm::translate(mat4(1.0f), pos), size); }

void Scene::init() {
    cube_ = Mesh::cube();
    woodT_ = Texture::wood(); wallT_ = Texture::wall();
    floorT_ = Texture::floorTiles(); hiddenT_ = Texture::hiddenMessage("CODE 4271");

    blockers_ = {
        {deskPos_ - vec3(1.0f, 0.4f, 0.45f), deskPos_ + vec3(1.0f, 0.4f, 0.45f)},      // desk
        {vec3(2.6f, 0.0f, -3.4f), vec3(3.8f, 1.8f, -2.6f)},                              // cabinet
        {vec3(2.2f, 0.0f, 2.2f),  vec3(2.8f, 0.6f, 2.8f)},                               // crate 1
        {vec3(-3.3f, 0.0f, 2.5f), vec3(-2.7f, 0.5f, 3.1f)},                              // crate 2
        {vec3(-4.0f, 0.0f, -1.0f), vec3(-3.5f, 2.0f, 1.0f)},                            // bookshelf
        {vec3(-3.3f, 0.0f, -2.5f), vec3(-2.7f, 0.9f, -1.9f)},                            // chair
    };
}

bool Scene::walkable(const vec3& p) const {
    const float r = 0.3f;
    if (p.x > 3.7f) {                                   // doorway to the outside
        if (!doorOpen_ || std::fabs(p.z) > 0.45f || p.x > 5.0f) return false;
    }
    else if (p.x < -3.7f || std::fabs(p.z) > 3.7f) return false;
    for (const Box& b : blockers_)
        if (p.x > b.min.x - r && p.x < b.max.x + r && p.z > b.min.z - r && p.z < b.max.z + r) return false;
    return true;
}

void Scene::move(vec3& pos, const vec3& d) const {      // axis-separated => sliding along walls
    vec3 t = pos; t.x += d.x; if (walkable(t)) pos.x = t.x;
    t = pos;      t.z += d.z; if (walkable(t)) pos.z = t.z;
}

void Scene::update(float dt) {
    auto approach = [dt](float& v, float target, float speed) {
        if (v < target) v = std::min(v + speed * dt, target); else v = std::max(v - speed * dt, target);
        };
    approach(drawerT_, drawerOpen_ ? 1.0f : 0.0f, 2.0f);
    approach(doorT_, doorOpen_ ? 1.0f : 0.0f, 0.8f);
    if (messageTimer_ > 0) messageTimer_ -= dt;
    if (!finished_) playTime_ += dt;
    if (wrongTimer_ > 0) wrongTimer_ -= dt;
}

void Scene::interact(const vec3& p) {
    auto dist2D = [&](float x, float z) { return std::hypot(p.x - x, p.z - z); };
    if (dist2D(deskPos_.x, deskPos_.z + 0.45f) < 1.6f) {
        if (!drawerOpen_) { drawerOpen_ = true; say("Drawer opened"); }
        else if (!keyTaken_) { keyTaken_ = true;  say("You took the key!"); }
        else { drawerOpen_ = false; say("Drawer closed"); }
    }
    else if (dist2D(3.9f, 0.0f) < 1.8f) {
        if (doorOpen_)         say("The door is already open");
        else if (!keyTaken_)   say("Locked. Find the key first");
        else if (!codeSolved_) { keypadOpen_ = true; input_.clear(); }   // key fits: now a code panel appears
        else                   doorOpen_ = true;
    }
}

std::string Scene::hint(const vec3& p) const {
    if (messageTimer_ > 0) return message_;
    if (std::hypot(p.x - deskPos_.x, p.z - (deskPos_.z + 0.45f)) < 1.6f) return "[E] use drawer";
    if (std::hypot(p.x - 3.9f, p.z) < 1.8f) {
        if (doorOpen_) return "";
        return keyTaken_ ? "[E] enter the code (look for it with the UV light)" : "[E] use door";
    }
    return "";
}

void Scene::drawItem(const Shader& sh, const Item& it) const {
    sh.setMat4("model", it.model);
    sh.setVec3("objColor", it.color);
    float density = (it.tex == floorT_ || it.tex == wallT_) ? 0.5f : 1.0f;   // texture repeats per metre
    sh.setVec2("uvScale", vec2(density));
    sh.setBool("worldUV", !it.hidden);                                       // the UV board keeps plain 0..1 UVs
    sh.setFloat("shininess", it.shininess);
    sh.setBool("useTex", it.tex != 0);
    sh.setBool("useHidden", it.hidden);
    sh.setBool("emissive", it.emissive);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, it.tex);
    cube_.draw();
}

void Scene::draw(const Shader& sh, const Camera& cam, float aspect) const {
    sh.use();
    sh.setMat4("view", cam.view());
    sh.setMat4("projection", cam.projection(aspect));
    sh.setVec3("viewPos", cam.position);
    sh.setInt("tex", 0); sh.setInt("hiddenTex", 1);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, hiddenT_);

    sh.setBool("lampOn", lampOn);   sh.setVec3("lampPos", lampPos_);
    sh.setBool("flashOn", flashOn);
    sh.setVec3("flashPos", cam.position - vec3(0.0f, 0.1f, 0.0f) + cam.front() * 0.1f);
    sh.setVec3("flashDir", cam.front());
    sh.setFloat("innerCut", std::cos(glm::radians(11.0f)));
    sh.setFloat("outerCut", std::cos(glm::radians(19.0f)));

    const vec3 white(1.0f);
    // ---- room shell ----
    drawItem(sh, { TRS({0, -0.05f, 0}, {8.4f, 0.1f, 8.4f}), white, floorT_, {4, 4}, 16 });
    drawItem(sh, { TRS({0, 3.05f, 0},  {8.4f, 0.1f, 8.4f}), white * 0.8f, wallT_, {4, 4}, 4 });
    drawItem(sh, { TRS({0, 1.5f, -4.1f}, {8.4f, 3.0f, 0.2f}), white, wallT_, {4, 1.5f}, 8 });   // north
    drawItem(sh, { TRS({0, 1.5f,  4.1f}, {8.4f, 3.0f, 0.2f}), white, wallT_, {4, 1.5f}, 8 });   // south
    drawItem(sh, { TRS({-4.1f, 1.5f, 0}, {0.2f, 3.0f, 8.4f}), white, wallT_, {4, 1.5f}, 8 });   // west
    drawItem(sh, { TRS({4.1f, 1.5f, -2.35f}, {0.2f, 3.0f, 3.7f}), white, wallT_, {2, 1.5f}, 8 }); // east A
    drawItem(sh, { TRS({4.1f, 1.5f,  2.35f}, {0.2f, 3.0f, 3.7f}), white, wallT_, {2, 1.5f}, 8 }); // east B
    drawItem(sh, { TRS({4.1f, 2.55f, 0.0f},  {0.2f, 0.9f, 1.0f}), white, wallT_, {1, 0.5f}, 8 }); // lintel

    // ---- the blank board with hidden UV writing ----
    drawItem(sh, { TRS({0, 1.7f, -3.97f}, {3.2f, 0.8f, 0.04f}), vec3(0.45f, 0.47f, 0.5f), 0, {1, 1}, 4, true });

    // ---- door: child of a hinge at (4.1, *, -0.5); rotation about the hinge ----
    mat4 hinge = glm::translate(mat4(1.0f), vec3(4.1f, 1.05f, -0.5f));
    hinge = glm::rotate(hinge, glm::radians(-90.0f * doorT_), vec3(0, 1, 0));
    drawItem(sh, { glm::scale(glm::translate(hinge, vec3(0, 0, 0.5f)), vec3(0.15f, 2.1f, 1.0f)),
                  white * 0.9f, woodT_, {1, 2}, 24 });
    // door handle (child of the same hinge matrix)
    drawItem(sh, { glm::scale(glm::translate(hinge, vec3(-0.1f, -0.1f, 0.85f)), vec3(0.08f, 0.08f, 0.08f)),
                  vec3(0.8f, 0.7f, 0.2f), 0, {1, 1}, 96 });

    // ---- desk (parent) + drawer and lamp (children) ----
    mat4 desk = glm::translate(mat4(1.0f), deskPos_);
    drawItem(sh, { glm::scale(desk, vec3(2.0f, 0.8f, 0.9f)), white, woodT_, {2, 1}, 24 });
    mat4 drawerM = glm::translate(desk, vec3(0.0f, 0.1f, 0.22f + 0.45f * drawerT_));
    drawItem(sh, { glm::scale(drawerM, vec3(0.9f, 0.25f, 0.5f)), white * 0.8f, woodT_, {1, 1}, 24 });
    if (drawerOpen_ && !keyTaken_)                                                         // key inside the drawer
        drawItem(sh, { glm::scale(glm::translate(drawerM, vec3(0, 0.14f, 0)), vec3(0.12f, 0.03f, 0.25f)),
                      vec3(1.0f, 0.85f, 0.1f), 0, {1, 1}, 64, false, true });
    // lamp: base + emissive bulb, light source at lampPos_
    drawItem(sh, { glm::scale(glm::translate(desk, vec3(-0.7f, 0.45f, 0)), vec3(0.2f, 0.1f, 0.2f)), vec3(0.15f), 0, {1, 1}, 32 });
    drawItem(sh, { glm::scale(glm::translate(desk, vec3(-0.7f, 0.65f, 0)), vec3(0.14f, 0.2f, 0.14f)),
                  lampOn ? vec3(1.0f, 0.85f, 0.5f) : vec3(0.25f), 0, {1, 1}, 32, false, true });

    // ---- cabinet and crates ----
    drawItem(sh, { TRS({3.2f, 0.9f, -3.0f}, {1.2f, 1.8f, 0.8f}), white * 0.85f, woodT_, {1, 2}, 24 });
    drawItem(sh, { TRS({2.5f, 0.3f, 2.5f},  {0.6f, 0.6f, 0.6f}), white, woodT_, {1, 1}, 16 });
    drawItem(sh, { TRS({-3.0f, 0.25f, 2.8f}, {0.6f, 0.5f, 0.6f}), white * 0.9f, woodT_, {1, 1}, 16 });

    // ---- decoration: rug, chair, bookshelf with books, painting ----
    drawItem(sh, { TRS({0, 0.01f, 0.5f}, {3.0f, 0.02f, 2.2f}), vec3(0.45f, 0.08f, 0.10f), 0, {1, 1}, 4 });   // rug

    const vec3 chair(-3.0f, 0.0f, -2.2f), chairWood(0.55f, 0.35f, 0.20f);                                   // chair
    drawItem(sh, { TRS(chair + vec3(0, 0.45f, 0), {0.5f, 0.06f, 0.5f}), chairWood, woodT_, {1, 1}, 16 });
    for (int sx = -1; sx <= 1; sx += 2) for (int sz = -1; sz <= 1; sz += 2)
        drawItem(sh, { TRS(chair + vec3(0.21f * sx, 0.21f, 0.21f * sz), {0.06f, 0.42f, 0.06f}), chairWood * 0.8f, woodT_, {1, 1}, 16 });
    drawItem(sh, { TRS(chair + vec3(0, 0.75f, 0.22f), {0.5f, 0.5f, 0.05f}), chairWood, woodT_, {1, 1}, 16 });

    drawItem(sh, { TRS({-3.75f, 1.0f, 0.0f}, {0.5f, 2.0f, 2.0f}), white * 0.55f, woodT_, {1, 2}, 12 });       // bookshelf
    auto fr = [](float x) { return x - std::floor(x); };
    for (int row = 0; row < 3; ++row) for (int i = 0; i < 10; ++i) {                                         // books (coloured spines)
        float f = fr(std::sin(i * 12.9898f + row * 78.233f) * 43758.5453f);
        float h = 0.30f + 0.08f * fr(f * 5.0f);
        vec3 col(0.30f + 0.5f * f, 0.20f + 0.5f * fr(f * 7.0f), 0.25f + 0.5f * fr(f * 13.0f));
        drawItem(sh, { TRS({-3.475f, 0.15f + 0.6f * row + h * 0.5f + 0.2f, -0.85f + 0.19f * i}, {0.05f, h, 0.15f}), col, 0, {1, 1}, 8 });
    }

    drawItem(sh, { TRS({0, 1.7f, 3.97f}, {1.4f, 0.9f, 0.06f}), vec3(0.30f, 0.20f, 0.10f), woodT_, {1, 1}, 16 });   // painting frame
    drawItem(sh, { TRS({0, 1.7f, 3.935f}, {1.2f, 0.7f, 0.02f}), vec3(0.15f, 0.45f, 0.50f), 0, {1, 1}, 8 });         // canvas
}

// ---------------- code lock ----------------
void Scene::keypadDigit(char c) {
    if (input_.size() < correctCode_.size()) input_.push_back(c);
}
void Scene::keypadBackspace() {
    if (!input_.empty()) input_.pop_back();
}
void Scene::keypadCancel() {
    keypadOpen_ = false; input_.clear();
}
void Scene::keypadConfirm() {
    if (input_ == correctCode_) {
        codeSolved_ = true; keypadOpen_ = false; doorOpen_ = true; input_.clear();
        say("Code accepted - the door is open!");
    }
    else {
        input_.clear();
        wrongTimer_ = 1.2f;                      // HUD shows WRONG CODE while this runs
    }
}

// ---------------- win screen / restart ----------------
void Scene::checkWin(const vec3& p) {
    if (!finished_ && escaped(p)) { finished_ = true; finalTime_ = playTime_; }
}

void Scene::reset() {
    drawerOpen_ = keyTaken_ = doorOpen_ = false;
    keypadOpen_ = codeSolved_ = false;
    finished_ = false;
    drawerT_ = doorT_ = 0.0f;
    playTime_ = finalTime_ = 0.0f;
    messageTimer_ = wrongTimer_ = 0.0f;
    input_.clear(); message_.clear();
}
