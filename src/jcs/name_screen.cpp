#include <jacky-common/message_writer.hpp>
#include <jacky-common/server_actions.hpp>
#include <jcs/game.hpp>
#include <jcs/game_screen.hpp>
#include <jcs/modal_screen.hpp>
#include <jcs/name_screen.hpp>

NameScreen::NameScreen(JSGame &game) :
    Screen{game},
    join_button{*this, 0, 0, 200, 50, [](GuiObject &o) {
        o.x = o.screen.game.window_width / 2.f + 25;
        o.y = o.screen.game.window_height / 2.f + 50;
    }, [](GuiButton &b) {
        const auto &name = dynamic_cast<NameScreen &>(b.screen).name_input.
            value;
        if (AddPlayerData::validate(name))
            b.screen.game.connection->join(name);
    }, L"Rejoindre"},

    cancel_button{*this, 0, 0, 200, 50, [](GuiObject &o) {
        o.x = o.screen.game.window_width / 2.f - 225;
        o.y = o.screen.game.window_height / 2.f + 50;
    }, [](GuiButton &b) {
        b.screen.game.connection->disconnect();
        b.screen.game.removeTopScreen();
        throw std::runtime_error{"bad programming"};
    }, L"Annuler"},

    name_input{*this, 0, 0, 450, 50, [](GuiObject &o) {
        o.x = o.screen.game.window_width / 2.f - 225;
        o.y = o.screen.game.window_height / 2.f - 25;
    }, nullptr} {
    gui_objects.push_back(&name_input);
    gui_objects.push_back(&cancel_button);
    gui_objects.push_back(&join_button);

    focusObject(&name_input);
}

void NameScreen::update() {
    {
        ws::lib::unique_lock<ws::lib::mutex> lock{
            game.connection->actions_lock};

        while (!game.connection->actions.empty()) {
            const auto action = std::move(game.connection->actions.front());
            game.connection->actions.pop();

            if (REFUSE_JOIN == action.type) {
                game.showScreen(new ModalScreen{
                    game, action.get<RefuseJoinData>()->message,
                    {{L"OK", [](GuiButton &b) {
                        b.screen.game.removeTopScreen();
                    }}}, 0
                });
                return;
            } else if (ALLOW_JOIN == action.type) {
                game.showScreen(
                    new GameScreen{game, *action.get<AllowJoinData>()}, true);
                return;
            }
        }
    }

    Screen::update();
}

void NameScreen::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const std::wstring msg{L"Choisissez un pseudo :"};
    const float l = game.renderer.default_font.getTextWidth(msg, 24);
    game.renderer.drawText(msg,
                           (game.window_width - l) / 2.f,
                           game.window_height / 2.f - 100,
                           24);

    Screen::render();
}

bool NameScreen::onKey(int key, int scancode, int action, int mods) {
    if (game.inputs.is(key, JSINPUT_ESCAPE) && GLFW_PRESS == action) {
        game.connection->disconnect();
        game.removeTopScreen();
        return true;
    }

    if (GLFW_KEY_ENTER == key && GLFW_PRESS == action &&
        !cancel_button.focused) {
        const auto &name = name_input.value;
        if (AddPlayerData::validate(name))
            game.connection->join(name);
        return true;
    }

    return Screen::onKey(key, scancode, action, mods);
}
