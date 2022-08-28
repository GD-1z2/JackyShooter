#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

class Camera {
public:
    enum Mode {
        NORMAL, THIRDP
    };

    explicit Camera(glm::vec3 position = {0.f, 0.f, 0.f},
                    glm::vec3 up = {0.0f, 1.0f, 0.0f},
                    float yaw = -90.f,
                    float pitch = 0.f);

    [[nodiscard]] glm::mat4 getViewMatrix() const;

    void processMouseMovement(float x_offset, float y_offset);

    void updateVectors();

    void switchMode();

    Mode mode{NORMAL};
    glm::vec3 position, front, flat_front, up, right;
    float yaw, pitch;
};
