#pragma once

#include <jacky-common/client_actions.hpp>
#include <jacky-common/defs.hpp>
#include <jcs/camera.hpp>
#include <jcs/chat.hpp>
#include <jcs/player_controller.hpp>
#include <jcs/screen.hpp>
#include <jcs/skybox.hpp>

class GameScreen : public Screen {
public:
    GameScreen(JSGame &game, const AllowJoinData &join_data);

    ~GameScreen() override;

    void update() override;

    void render() override;

    void onResize() override;

    void onFocus() override;

    void onClick(int button, int action, int mods) override;

    bool onKey(int key, int scancode, int action, int mods) override;

    void onChar(uint codepoint) override;


    bool display_overlays{true};
private:
    Chat chat;
    SkyBox skybox;

    PlayerController player_controller;
    std::chrono::high_resolution_clock::time_point
        last_pos_sync_c, last_pos_sync_s;
    Camera camera;
};
