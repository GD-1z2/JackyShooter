#pragma once

#include <glm/glm.hpp>
#include <jcs/camera.hpp>

class JSGame;

class PlayerController {
public:
    PlayerController(JSGame &game, Camera &camera);

    void update();


    glm::vec3 position;

private:
    JSGame &game;
    Camera &camera;
    float speed;
};
