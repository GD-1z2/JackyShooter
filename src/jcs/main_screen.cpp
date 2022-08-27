#include "glad/glad.hpp"

#include <codecvt>
#include <jcs/game.hpp>
#include <jcs/game_screen.hpp>
#include <jcs/main_screen.hpp>
#include <jcs/modal_screen.hpp>

MainScreen::MainScreen(JSGame &game) :
    Screen{game},
    play_button{new GuiTextButton{
        *this, 0, 0, 300, 50,
        [](GuiObject &obj) {
            obj.x = (float) obj.screen.game.window_width / 2.f - 150.f;
            obj.y = (float) obj.screen.game.window_height / 2.f;
        },
        [](GuiButton &obj) {
            using convert_type = std::codecvt_utf8<wchar_t>;
            std::wstring_convert<convert_type, wchar_t> converter;
            const std::string ip_str = converter.to_bytes(
                ((MainScreen &) obj.screen).ip_input->value);
            obj.screen.game.showScreen(new GameScreen{obj.screen.game, ip_str},
                                       true);
            throw std::runtime_error{"bad programming"};
        },
        L"Connect",
    }},
    ip_input{new GuiTextInput{
        *this, 0, 0, 300, 50,
        [](GuiObject &obj) {
            obj.x = (float) obj.screen.game.window_width / 2.f - 150.f;
            obj.y = (float) obj.screen.game.window_height / 2.f + 75.f;
        }, nullptr
    }},
    name_input{new GuiTextInput{
        *this, 0, 0, 300, 50,
        [](GuiObject &obj) {
            obj.x = (float) obj.screen.game.window_width / 2.f - 150.f;
            obj.y = (float) obj.screen.game.window_height / 2.f + 150.f;
        }, nullptr
    }} {

    ip_input->value = L"ws://localhost:9002";
    name_input->value = L"zeubi";

    gui_objects.push_back(play_button);
    gui_objects.push_back(ip_input);
    gui_objects.push_back(name_input);

    glfwSetInputMode(game.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glClearColor(0.6588235294117647f, 0.19607843137254902f, 0.4117647058823529f,
                 1.f); // horrible pink


    title_vbo = VertexBuffer{VertexBuffer::BUF_DYN};
    MainScreen::onResize();
}

MainScreen::~MainScreen() {
    delete play_button;
    delete ip_input;
    delete name_input;
}

void MainScreen::update() {
    Screen::update();
}

void MainScreen::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    game.renderer.set2D();

    Screen::render();

    game.renderer.clearTransform();

    title_vbo.draw();
}

void MainScreen::onFocus() {
    glfwSetInputMode(game.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void MainScreen::onClick(int button, int action, int mods) {
    Screen::onClick(button, action, mods);
}

bool MainScreen::onKey(int key, int scancode, int action, int mods) {
    if (Screen::onKey(key, scancode, action, mods))
        return true;

    if (game.inputs.is(key, JSINPUT_ESCAPE) && action == GLFW_PRESS) {
        game.showScreen(
            new ModalScreen{game, L"Are you sure you want to quit?", {
                {L"Yes", [](GuiButton &button) {
                    glfwSetWindowShouldClose(button.screen.game.window, true);
                }},
                {L"Cancel",
                 [](GuiButton &button) { button.screen.game.removeTopScreen(); }
                },
            }, 0});
        return true;
    }

    return false;
}

void MainScreen::onChar(uint codepoint) {
    Screen::onChar(codepoint);
}

void MainScreen::onResize() {
    Screen::onResize();

    std::vector<float> title_buff(12 * 30);
    game.renderer.default_font.writeStringDataRightAligned(
        title_buff.data(), L"Jacky", game.window_width - 48, 0,
        36);
    game.renderer.default_font.writeStringDataRightAligned(
        title_buff.data() + 5 * 30, L"Shooter", game.window_width - 48, 48,
        48);
    title_vbo.load(title_buff);
}
