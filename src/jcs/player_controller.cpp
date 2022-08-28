#include <jcs/player_controller.hpp>
#include <jcs/game.hpp>

PlayerController::PlayerController(JSGame &game, Camera &camera) :
    game{game}, camera{camera}, position{0}, speed{3.f} {}

void PlayerController::update() {
    const float distance = game.delta * speed;

    if (game.inputs.getInput(JSINPUT_FORWARD))
        position += camera.flat_front * distance;

    if (game.inputs.getInput(JSINPUT_BACKWARD))
        position -= camera.flat_front * distance * .6f;

    if (game.inputs.getInput(JSINPUT_LEFT))
        position -= camera.right * distance * .7f;

    if (game.inputs.getInput(JSINPUT_RIGHT))
        position += camera.right * distance * .7f;

    if (game.inputs.getInput(JSINPUT_JUMP))
        position.y += distance;

    if (game.inputs.getInput(JSINPUT_CROUCH))
        position.y -= distance;

    camera.position = position + glm::vec3{0, 1.75f, 0};
}
