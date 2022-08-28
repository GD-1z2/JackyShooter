#include <jcs/input_manager.hpp>
#include <jcs/game.hpp>

InputManager::InputManager(JSGame &game) : game{game}, bindings{
    {GLFW_KEY_W},          // JSINPUT_FORWARD
    {GLFW_KEY_S},          // JSINPUT_BACKWARD
    {GLFW_KEY_A},          // JSINPUT_LEFT
    {GLFW_KEY_D},          // JSINPUT_RIGHT
    {GLFW_KEY_SPACE},      // JSINPUT_JUMP
    {GLFW_KEY_LEFT_SHIFT}, // JSINPUT_CROUCH
    {GLFW_KEY_ESCAPE},     // JSINPUT_ESCAPE
    {GLFW_KEY_T},          // JSINPUT_CHAT
    {GLFW_KEY_C},          // JSINPUT_CAMERA_MODE
} {}

void InputManager::update() {
    for (auto &binding: bindings) {
        binding.previous = binding.current;

        if (binding.is_mouse)
            binding.current = glfwGetMouseButton(game.window, binding.id);
        else
            binding.current = glfwGetKey(game.window, binding.id);
    }

    double mouse_x_d, mouse_y_d;
    glfwGetCursorPos(game.window, &mouse_x_d, &mouse_y_d);

    mouse_x = static_cast<float>(mouse_x_d);
    mouse_y = static_cast<float>(mouse_y_d);

    if (first_mouse) {
        last_x = mouse_x;
        last_y = mouse_y;
        first_mouse = false;
    }

    mouse_x_offset = mouse_x - last_x;
    mouse_y_offset = last_y - mouse_y;

    last_x = mouse_x;
    last_y = mouse_y;
}

int InputManager::getInput(InputBindings input) const {
    return bindings[input].current;
}

int InputManager::getPreviousInput(InputBindings input) const {
    return bindings[input].previous;
}

bool InputManager::is(int key, InputBindings input) const {
    return bindings[input].id == key;
}

// GLFW Callbacks

void InputManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    auto &game{*static_cast<JSGame *>(glfwGetWindowUserPointer(window))};
    game.getScreen().onClick(button, action, mods);
}

void InputManager::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto &game{*static_cast<JSGame *>(glfwGetWindowUserPointer(window))};
    game.getScreen().onKey(key, scancode, action, mods);
}

void InputManager::charCallback(GLFWwindow *window, uint codepoint) {
    auto &game{*static_cast<JSGame *>(glfwGetWindowUserPointer(window))};
    game.getScreen().onChar(codepoint);
}
