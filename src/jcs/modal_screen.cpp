#include <glm/ext/matrix_transform.hpp>
#include <jcs/game.hpp>
#include <jcs/modal_screen.hpp>

ModalScreen::ModalScreen(JSGame &game, std::wstring message, const std::vector<Button> &buttons,
                         int main_button_index) :
    Screen{game}, parent_screen{game.getScreen()}, message{std::move(message)} {

    for (int i{0}; i < buttons.size(); i++) {
        const auto &[button_message, click_handler] = buttons[i];

        gui_objects.push_back(new GuiTextButton{
            *this, 0, game.window_height / 2.f + (float) i * 75.f, 300, 50,
            [](GuiObject &obj) {
                obj.x = (float) obj.screen.game.window_width / 2.f - 150.f;
            },
            click_handler,
            button_message,
        });
    }

    if (main_button_index >= 0) {
        assert(main_button_index < buttons.size());
        main_button = gui_objects[main_button_index];
    }

    if (main_button)
        focusObject(main_button);
}

void ModalScreen::update() {
    Screen::update();
}

void ModalScreen::render() {
    parent_screen.render();

    game.renderer.set2D();
    game.renderer.useColor({0, 0, 0, .6});
    game.renderer.setTransform({0, 0, 0}, {game.window_width, game.window_height, 1});
    game.renderer.rect_vbo.draw();

    Screen::render(); // draw buttons

    game.renderer.clearTransform();
    game.renderer.drawText(message,
                           (game.window_width - game.renderer.getTextWidth(message, 24)) / 2.f,
                           game.window_height / 4.f, 24);
}

void ModalScreen::onResize() {
    parent_screen.onResize();

    Screen::onResize();

    for (int i{0}; i < gui_objects.size(); i++)
        gui_objects[i]->y = game.window_height / 2.f + (float) i * 75.f;
}

void ModalScreen::onFocus() {
    glfwSetInputMode(game.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool ModalScreen::onKey(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        game.removeTopScreen();
        return true;
    }

    return Screen::onKey(key, scancode, action, mods);
}
