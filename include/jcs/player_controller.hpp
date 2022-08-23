#pragma once

#include <glm/glm.hpp>
#include <jcs/camera.hpp>

class JSGame;

class PlayerController {
public:
    explicit PlayerController(JSGame &game, Camera &camera);

    void update();

private:
    JSGame &game;
    Camera &camera;
    glm::vec3 position;
    float speed;
};
