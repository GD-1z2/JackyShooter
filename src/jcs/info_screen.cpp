#include <jcs/info_screen.hpp>
#include <jcs/game.hpp>

InfoScreen::InfoScreen(JSGame &game) :
    Screen{game},
    parent_screen{dynamic_cast<GameScreen &>(game.getScreen())} {

    parent_screen.display_overlays = false;
}

InfoScreen::~InfoScreen() noexcept {
    parent_screen.display_overlays = true;
}

void InfoScreen::update() {
    parent_screen.update();
}

void InfoScreen::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    parent_screen.render();

    game.renderer.set2D();

    game.renderer.gui_shader.useColor({0, 0, 0, .3});
    game.renderer.setTransform({0, 0, 0},
                               {game.window_width, game.window_height, 1});
    game.renderer.rect_vbo.draw();

    game.renderer.clearTransform();

    game.renderer.drawText(
        L"Info",
        (game.window_width - game.renderer.getTextWidth(L"Info", 48)) / 2,
        25,
        48);

    for (size_t i{0}; i < game.game_state.players.size(); i++) {
        auto &player = game.game_state.players[i];
        game.renderer.drawText(
            player.name,
            (game.window_width
             - game.renderer.getTextWidth(player.name, 12)) / 2,
            150 + static_cast<float>(i) * 30,
            12);
    }
}

void InfoScreen::onFocus() {
    glfwSetInputMode(game.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool InfoScreen::onKey(int key, int scancode, int action, int mods) {
    if (game.inputs.is(key, JSINPUT_ESCAPE) && GLFW_PRESS == action) {
        game.removeTopScreen();
        return true;
    }

    return Screen::onKey(key, scancode, action, mods);
}
