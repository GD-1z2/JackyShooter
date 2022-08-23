#include <GLFW/glfw3.h>
#include <jcs/game.hpp>

int main() {
    JSGame game{};

    while (!glfwWindowShouldClose(game.window)) {
        game.update();
        game.render();
        glfwSwapBuffers(game.window);
        glfwPollEvents();
    }

    return 0;
}
