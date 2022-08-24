#pragma once

#include <GLFW/glfw3.h>
#include <jacky-common/types.hpp>
#include <jcs/input_manager.hpp>
#include <jcs/renderer.hpp>
#include <jcs/screen.hpp>
#include <jcs/shader.hpp>
#include <queue>
#include <stack>

class JSGame {
public:
    JSGame();

    ~JSGame();

    void update();

    void render();

    void showScreen(Screen *screen, bool clear_stack = false);

    void removeTopScreen();

    Screen &getScreen() const;

    GLFWwindow *window;
    float window_width{SCR_WIDTH}, window_height{SCR_HEIGHT};

    Renderer renderer;

    InputManager input_manager;

    float delta{}, frame_last{}, frame_timer, now;
    int frame_counter{}, frames_per_sec{};

private:
    /* not using an actual stack because we need to access non-top screens */
    std::vector<Screen *> screen_stack;
};
