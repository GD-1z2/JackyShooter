#pragma once

#include <GLFW/glfw3.h>
#include <jacky-common/types.hpp>
#include <jcs/defs.hpp>

#define INPUT_COUNT 7

enum InputBindings {
    JSINPUT_FORWARD,
    JSINPUT_BACKWARD,
    JSINPUT_LEFT,
    JSINPUT_RIGHT,
    JSINPUT_JUMP,
    JSINPUT_CROUCH,
    JSINPUT_ESCAPE,
};

struct InputBinding {
    int id{GLFW_KEY_UNKNOWN};
    bool is_mouse{false};
    int current{GLFW_RELEASE}, previous{GLFW_RELEASE};
};

class JSGame;

class InputManager {
public:
    explicit InputManager(JSGame &game);

    void update();

    [[nodiscard]] int getInput(InputBindings input) const;

    [[nodiscard]] int getPreviousInput(InputBindings input) const;

    // GLFW Callbacks

    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void charCallback(GLFWwindow* window, uint codepoint);

    float
        mouse_x{},
        mouse_y{},
        last_x{SCR_WIDTH / 2.f},
        last_y{SCR_HEIGHT / 2.f},
        mouse_x_offset{},
        mouse_y_offset{};
private:
    JSGame &game;
    InputBinding bindings[INPUT_COUNT];

    bool first_mouse{true};
};
