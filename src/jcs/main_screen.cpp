#include <glad/glad.hpp>

#include <codecvt>
#include <jcs/game.hpp>
#include <jcs/main_screen.hpp>
#include <jcs/modal_screen.hpp>
#include <jcs/name_screen.hpp>

MainScreen::MainScreen(JSGame &game) :
    Screen{game},
    play_button{
        *this, 0, 0, 300, 50,
        [](GuiObject &obj) {
            obj.x = (float) obj.screen.game.window_width / 2.f - 150.f;
            obj.y = (float) obj.screen.game.window_height / 2.f;
        },
        [](GuiButton &obj) {
            using convert_type = std::codecvt_utf8<wchar_t>;
            std::wstring_convert<convert_type, wchar_t> converter;
            const std::string ip_str = converter.to_bytes(
                ((MainScreen &) obj.screen).ip_input.value);
            obj.screen.game.connection = new Connection{ip_str};
        },
        L"Connect",
    },
    ip_input{
        *this, 0, 0, 300, 50,
        [](GuiObject &obj) {
            obj.x = (float) obj.screen.game.window_width / 2.f - 150.f;
            obj.y = (float) obj.screen.game.window_height / 2.f + 75.f;
        }, nullptr
    } {

    ip_input.value = L"ws://localhost:9002";

    gui_objects.push_back(&play_button);
    gui_objects.push_back(&ip_input);

    glfwSetInputMode(game.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glClearColor(0.6588235294117647f, 0.19607843137254902f, 0.4117647058823529f,
                 1.f); // horrible pink


    title_vbo = VertexBuffer{VertexBuffer::BUF_DYN};
    MainScreen::onResize();
}

void MainScreen::update() {
    if (game.connection) {
        ws::lib::unique_lock<ws::lib::mutex> lock{
            game.connection->actions_lock};

        while (!game.connection->actions.empty()) {
            const ClientAction action = std::move(
                game.connection->actions.front());
            game.connection->actions.pop();

            if (CONNECT == action.type) {
                game.game_state = GameState{};
                game.showScreen(new NameScreen{game});
            }
        }
    }

    Screen::update();
}

void MainScreen::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    game.renderer.set2D();

    Screen::render();

    game.renderer.clearTransform();

    title_vbo.draw();
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
