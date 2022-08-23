#pragma once

#include <jcs/screen.hpp>
#include <utility>

class ModalScreen : public Screen {
public:
    typedef std::tuple<std::wstring, GuiButton::ClickHandler> Button;

    /**
     * @brief Modal screen overlay
     *
     * @note Must be created with a parent screen
     */
    ModalScreen(JSGame &game, std::wstring message, const std::vector<Button> &buttons, int main_button_index = -1);

    void update() override;

    void render() override;

    void onResize() override;

    void onFocus() override;

    bool onKey(int key, int scancode, int action, int mods) override;

    Screen &parent_screen;
    std::wstring message;
    GuiObject *main_button{nullptr};
};
