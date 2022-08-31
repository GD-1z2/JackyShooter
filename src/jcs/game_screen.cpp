#include <jacky-common/message_writer.hpp>
#include <jacky-common/server_actions.hpp>
#include <jcs/game.hpp>
#include <jcs/game_screen.hpp>
#include <jcs/info_screen.hpp>
#include <jcs/main_screen.hpp>
#include <jcs/modal_screen.hpp>

GameScreen::GameScreen(JSGame &game, const AllowJoinData &join_data) :
    Screen{game},
    camera{},
    player_controller{game, camera},
    chat{game},
    skybox{game.renderer, game.renderer.skybox_texture} {

    for (const auto &name: join_data.online_players)
        game.game_state.players.push_back({name, false});
    player_controller.position = join_data.spawn_pos;

    glClearColor(0, 0, 0, 1);

    chat.initRender();
}

GameScreen::~GameScreen() = default;

void GameScreen::update() {
    {
        ws::lib::unique_lock<ws::lib::mutex> lock{
            game.connection->actions_lock};

        while (!game.connection->actions.empty()) {
            const ClientAction action = std::move(
                game.connection->actions.front());
            game.connection->actions.pop();

            if (ADD_CHAT_MESSAGE == action.type) {
                chat.addMessage(std::get<std::unique_ptr<AddChatMessageData>>(
                    action.data)->message);

            } else if (PLAYER_LIST_ADD == action.type) {
                const auto &data = action.get<PlayerListAddData>();
                game.game_state.players.push_back({data->name, false});
                chat.addMessage(data->name + L" a rejoint le serveur");

            } else if (PLAYER_LIST_RMV == action.type) {
                const auto &data = action.get<PlayerListRmvData>();
                const auto it = std::find_if(game.game_state.players.begin(),
                                             game.game_state.players.end(),
                                             [&data](const ClientPlayer &p) {
                                                 return p.name == data->name;
                                             });
                if (it != game.game_state.players.end()) {
                    game.game_state.players.erase(it);
                    chat.addMessage(data->name + L" a quitté le serveur");
                }
            }
        }
    }

    Screen::update();

    if (game.isTopScreen(this)) {
        player_controller.update();

        camera.processMouseMovement(game.inputs.mouse_x_offset,
                                    game.inputs.mouse_y_offset);
    }
}

void GameScreen::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    game.renderer.useShader(WORLD_SHADER);
    game.renderer.set3D(camera.getViewMatrix());
    game.renderer.world_shader.setViewPos(camera.position);
    game.renderer.world_shader.setLightPos({-3, 0, -3});

    game.renderer.setTransform({0, 0, -3}, {.1, .1, .1});
    game.renderer.gun_model.draw();

    if (Camera::THIRDP == camera.mode) {
        game.renderer.setTransform(player_controller.position, {1, 1, 1},
                                   {0, -camera.yaw + 90, 0});
        game.renderer.player_model.draw();
    }

    skybox.draw(glm::lookAt(glm::vec3{}, camera.front, camera.up));

    game.renderer.useShader(GUI_SHADER);

    if (display_overlays) {
        chat.render();
    }

    Screen::render();
}

void GameScreen::onResize() {
    Screen::onResize();
    chat.updateBuffer();
}

void GameScreen::onFocus() {
    glfwSetCursorPos(game.window, game.window_width / 2,
                     game.window_height / 2);
    glfwSetInputMode(game.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    game.inputs.first_mouse = true;
}

void GameScreen::onClick(int button, int action, int mods) {
    Screen::onClick(button, action, mods);
}

bool GameScreen::onKey(int key, int scancode, int action, int mods) {
    if (game.inputs.is(key, JSINPUT_ESCAPE) && GLFW_PRESS == action) {
        game.showScreen(new ModalScreen{game, L"Pause menu", {
            {L"Resume", [](GuiButton &button) {
                button.screen.game.removeTopScreen();
                throw std::runtime_error{"bad programming"};
            }},
            {L"Disconnect", [](GuiButton &button) {
                button.screen.game.connection->disconnect();
                button.screen.game.showScreen(
                    new MainScreen{button.screen.game}, true);
                throw std::runtime_error{"bad programming"};
            }}
        }, 0 /*resume is default button*/});

        return true;
    }

    if (game.inputs.is(key, JSINPUT_CHAT) && GLFW_PRESS == action) {
        game.showScreen(new ChatScreen{game, chat});
        return true;
    }

    if (game.inputs.is(key, JSINPUT_INFO) && GLFW_PRESS == action) {
        game.showScreen(new InfoScreen{game});
        return true;
    }

    if (game.inputs.is(key, JSINPUT_CAMERA_MODE) && GLFW_PRESS == action) {
        camera.switchMode();
        return true;
    }

    return false;
}

void GameScreen::onChar(uint codepoint) {
}
