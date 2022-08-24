#include <jacky-common/message_writer.hpp>
#include <jacky-common/server_actions.hpp>
#include <jcs/game.hpp>
#include <jcs/game_screen.hpp>
#include <jcs/main_screen.hpp>
#include <jcs/modal_screen.hpp>

GameScreen::GameScreen(JSGame &game, const std::string &uri) :
    Screen{game},
    camera{},
    player_controller{game, camera},
    chat{game},
    skybox{game.renderer, game.renderer.skybox_texture} {

    if (!connect(uri))
        throw std::runtime_error{"WebSocket connection failed"};

    glClearColor(0, 0, 0, 1);

    chat.initRender();
}

GameScreen::~GameScreen() = default;

void GameScreen::update() {
    {
        ws::lib::unique_lock<ws::lib::mutex> lock{actions_queue_lock};

        while (!actions_queue.empty()) {
            const ClientAction action = std::move(actions_queue.front());
            actions_queue.pop();

            if (ADD_CHAT_MESSAGE == action.type)
                chat.addMessage(std::get<std::unique_ptr<AddChatMessageData>>(
                    action.data)->message);
        }
    }

    Screen::update();

    player_controller.update();

    camera.processMouseMovement(game.input_manager.mouse_x_offset,
                                game.input_manager.mouse_y_offset);
}

void GameScreen::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    game.renderer.useShader(WORLD_SHADER);
    game.renderer.set3D(camera.getViewMatrix());
    game.renderer.world_shader.setViewPos(camera.position);
    game.renderer.world_shader.setLightPos({-3, 0, -3});

    game.renderer.setTransform({0, 0, -3}, {.1, .1, .1});
    game.renderer.gun_model.draw();

    skybox.draw(glm::lookAt(glm::vec3{}, camera.front, camera.up));

    game.renderer.useShader(GUI_SHADER);

    chat.render();

    Screen::render();
}

void GameScreen::onFocus() {
    glfwSetCursorPos(game.window, game.window_width / 2,
                     game.window_height / 2);
    glfwSetInputMode(game.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    game.input_manager.first_mouse = true;
}

void GameScreen::onClick(int button, int action, int mods) {
    Screen::onClick(button, action, mods);

    MessageWriter msg;
    msg.writeShort(SEND_CHAT_MESSAGE);
    msg.writeArr<std::wstring>(L"Bonjour, monde");
    if (action) ws_connection->send(msg.data(), msg.size());
}

bool GameScreen::onKey(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        game.showScreen(new ModalScreen{game, L"Pause menu", {
            {L"Resume", [](GuiButton &button) {
                button.screen.game.removeTopScreen();
                throw std::runtime_error{"bad programming"};
            }},
            {L"Disconnect", [](GuiButton &button) {
                auto &screen = reinterpret_cast<GameScreen &>(
                    reinterpret_cast<ModalScreen &>(button.screen).parent_screen
                );

                // Terminate connection
                if (screen.ws_connection->get_state() ==
                    ws::session::state::open)
                    screen.ws_connection->close(ws::close::status::normal,
                                                "Disconnect");
                screen.ws_thread.join();

                screen.game.showScreen(new MainScreen{screen.game}, true);
            }}
        }, 0 /*resume is default button*/});

        return true;
    }

    return false;
}

void GameScreen::onChar(uint codepoint) {
}
