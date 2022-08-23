#include <jcs/camera.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    position{position},
    front{0.f, 0.f, -1.f},
    flat_front{front},
    up{up},
    right{0.f},
    yaw{yaw},
    pitch{pitch} {
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::processMouseMovement(float x_offset, float y_offset) {
    yaw += x_offset * .07f;
    pitch = glm::clamp(pitch + y_offset * .07f, -89.f, 89.f);

    updateVectors();
}

void Camera::updateVectors() {
    flat_front = glm::normalize(
        glm::vec3{
            (float) (cos(glm::radians(yaw)) * cos(glm::radians(pitch))),
            0.f,
            (float) (sin(glm::radians(yaw)) * cos(glm::radians(pitch)))
        }
    );
    front = glm::normalize(
        glm::vec3{
            (float) (cos(glm::radians(yaw)) * cos(glm::radians(pitch))),
            (float) (sin(glm::radians(pitch))),
            (float) (sin(glm::radians(yaw)) * cos(glm::radians(pitch)))
        }
    );
    right = glm::normalize(glm::cross(front, {0, 1, 0}));
    up = glm::normalize(glm::cross(right, front));
}
