#pragma once

#include <jacky-common/client_actions.hpp>
#include <jacky-common/defs.hpp>
#include <jacky-common/types.hpp>
#include <jcs/camera.hpp>
#include <jcs/chat.hpp>
#include <jcs/player_controller.hpp>
#include <jcs/screen.hpp>

class GameScreen : public Screen {
public:
    GameScreen(JSGame &game, const std::string &uri);

    ~GameScreen() override;

    void update() override;

    void render() override;

    void onFocus() override;

    void onClick(int button, int action, int mods) override;

    bool onKey(int key, int scancode, int action, int mods) override;

    void onChar(uint codepoint) override;


    bool connect(const std::string &uri);

private:
    PlayerController player_controller;
    Camera camera;
    Chat chat;
    WsClient ws_client;
    WsClient::connection_ptr ws_connection;
    std::thread ws_thread;
    std::queue<ClientAction> actions_queue;
    ws::lib::mutex actions_queue_lock;
};
