#include "glad/glad.hpp"
#include <jcs/game.hpp>
#include <jcs/input_manager.hpp>
#include <jcs/main_screen.hpp>
#include <jcs/texture.hpp>
#include <stdexcept>

JSGame::JSGame() : renderer{*this}, inputs{*this} {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Jacky Shooter", nullptr,
                              nullptr);
    if (nullptr == window)
        throw std::runtime_error{"Failed to create GLFW window"};

    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window_, int width,
                                              int height) {
        auto *game = static_cast<JSGame *>(glfwGetWindowUserPointer(window_));
        game->window_width = (float) width;
        game->window_height = (float) height;

        glViewport(0, 0, width, height);

        game->getScreen().onResize();
    });

    glfwSetMouseButtonCallback(window, InputManager::mouseButtonCallback);
    glfwSetKeyCallback(window, InputManager::keyCallback);
    glfwSetCharCallback(window, InputManager::charCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw std::runtime_error{"Failed to initialize GLAD"};

    renderer.init();

    now = static_cast<float>(glfwGetTime());
    frame_timer = now;

    screen_stack.push_back(new MainScreen{*this});
}

JSGame::~JSGame() = default;

void JSGame::update() {
    now = static_cast<float>(glfwGetTime());
    delta = now - frame_last;
    frame_last = now;

    if (now - frame_timer > 1) {
        frame_timer += 1;
        frames_per_sec = frame_counter;
        frame_counter = 0;
    }
    frame_counter++;

    inputs.update();

    screen_stack.back()->update();
}

void JSGame::render() {
    renderer.prepareRender();

    screen_stack.back()->render();

    renderer.useShader(GUI_SHADER);
    renderer.set2D();
    renderer.clearTransform();
    renderer.drawText(std::to_wstring(frames_per_sec) + L" fps", 10, 10);
}

void JSGame::showScreen(Screen *screen, bool clear_stack) {
    if (clear_stack) {
        while (!screen_stack.empty()) {
            delete screen_stack.back();
            screen_stack.pop_back();
        }
    }

    screen_stack.push_back(screen);
    screen->onFocus();
}

void JSGame::removeTopScreen() {
    delete screen_stack.back();
    screen_stack.pop_back();

    if (screen_stack.empty())
        screen_stack.push_back(new MainScreen{*this});

    screen_stack.back()->onFocus();
}

Screen &JSGame::getScreen() const {
    assert(!screen_stack.empty() && "No screen is currently active");
    return *screen_stack.back();
}
