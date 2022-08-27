#pragma once

#include <functional>
#include <string>
#include <utility>

class Screen;

struct GuiObject {
    typedef void (*ResizeHandler)(GuiObject &);

    GuiObject(Screen &screen,
              float x, float y,
              float width, float height,
              ResizeHandler resize_handler) :
        screen{screen}, x{x}, y{y}, width{width}, height{height},
        resize_handler{resize_handler} {
        resize_handler(*this);
    }

    virtual void update();

    virtual void render() = 0;

    virtual void onClick(int button, int action, int mods);

    virtual bool onKey(int key, int scancode, int action, int mods) {
        return false;
    }

    virtual void onChar(uint codepoint) {}

    Screen &screen;

    float x, y, width, height;
    bool hovered{};
    bool focused{};
    bool clicked{};
    ResizeHandler resize_handler;
};

struct GuiButton : public GuiObject {
    typedef void (*ClickHandler)(GuiButton &);

    GuiButton(Screen &screen, float x, float y, float width, float height,
              ResizeHandler resize_handler,
              ClickHandler click_handler) :
        GuiObject{screen, x, y, width, height, resize_handler},
        click_handler{click_handler} {}

    void onClick(int button, int action, int mods) override;

    bool onKey(int key, int scancode, int action, int mods) override;

    ClickHandler click_handler;
};

struct GuiTextButton : public GuiButton {
    GuiTextButton(Screen &screen, float x, float y, float width, float height,
                  ResizeHandler resize_handler,
                  ClickHandler click_handler, const std::wstring &text);

    void render() override;

    std::wstring text;
    float text_offset_x, text_offset_y;
};

struct GuiTextInput : public GuiObject {
    typedef void (*ChangeHandler)(GuiTextInput &, uint);

    GuiTextInput(Screen &screen, float x, float y, float width, float height,
                 ResizeHandler resize_handler,
                 ChangeHandler change_handler);

    void render() override;

    void onClick(int button, int action, int mods) override;

    bool onKey(int key, int scancode, int action, int mods) override;

    void onChar(uint codepoint) override;

    std::wstring value;
    float text_offset;
    ChangeHandler change_handler;
};
