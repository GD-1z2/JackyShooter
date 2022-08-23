#include <glm/gtc/matrix_transform.hpp>
#include <jcs/game.hpp>
#include <jcs/gui.hpp>

void GuiObject::update() {
    hovered = screen.game.input_manager.mouse_x > x &&
              screen.game.input_manager.mouse_x < x + width &&
              screen.game.input_manager.mouse_y > y &&
              screen.game.input_manager.mouse_y < y + height;
}

void GuiObject::onClick(int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (hovered)
            clicked = true;
    } else if (action == GLFW_RELEASE) {
        if (clicked && hovered)
            screen.focusObject(this);

        clicked = false;
    }
}


void GuiButton::onClick(int button, int action, int mods) {
    if (action == GLFW_RELEASE && clicked && hovered && click_handler)
        click_handler(*this);

    GuiObject::onClick(button, action, mods);
}

bool GuiButton::onKey(int key, int scancode, int action, int mods) {
    if (focused && (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE) && action) {
        click_handler(*this);
        return true;
    }

    return false;
}


GuiTextButton::GuiTextButton(Screen &screen, float x, float y, float width,
                             float height,
                             GuiObject::ResizeHandler resize_handler,
                             GuiButton::ClickHandler click_handler,
                             const std::wstring &text) :
    GuiButton{screen, x, y, width, height, resize_handler, click_handler},
    text{text},
    text_offset_x{(width - screen.game.renderer.getTextWidth(text, 24)) / 2.f},
    text_offset_y{
        (height - (float) screen.game.renderer.default_font.line_height) /
        2.f} {}

void GuiTextButton::render() {
    auto &renderer = screen.game.renderer;

    if (focused) {
        renderer.useColor({1, 1, 1, 1});
        renderer.setTransform({x - 2, y - 2, 0}, {width + 4, height + 4, 1});
        renderer.rect_vbo.draw();
    }

    renderer.useTexture(renderer.button_texture);
    renderer.setTransform({x, y, 0}, {width, height, 1});

    static const glm::mat4
        TC_NORMAL{glm::translate(
        glm::scale(glm::mat4{1.f}, {width / height, 1.f / 3.f, 1.f}),
        {0.f, 2.f, 0.f})},
        TC_HOVERED{glm::translate(
        glm::scale(glm::mat4{1.f}, {width / height, 1.f / 3.f, 1.f}),
        {0.f, 1.f, 0.f})},
        TC_CLICKED{
        glm::scale(glm::mat4{1.f}, {width / height, 1.f / 3.f, 1.f})};

    if (hovered)
        if (clicked)
            renderer.gui_shader.setTexCoordTransform(TC_CLICKED);
        else
            renderer.gui_shader.setTexCoordTransform(TC_HOVERED);
    else
        renderer.gui_shader.setTexCoordTransform(TC_NORMAL);

    renderer.rect_vbo.draw();

    renderer.gui_shader.setTexCoordTransform(glm::mat4{1.f});
    renderer.clearTransform();

    renderer.drawText(text, x + text_offset_x, y + text_offset_y, 24);
}


GuiTextInput::GuiTextInput(Screen &screen, float x, float y, float width,
                           float height,
                           GuiObject::ResizeHandler resize_handler,
                           GuiTextInput::ChangeHandler change_handler) :
    GuiObject{screen, x, y, width, height, resize_handler},
    change_handler{change_handler},
    text_offset{
        (height - (float) screen.game.renderer.default_font.line_height) /
        2.f} {}

void GuiTextInput::render() {
    const auto &renderer = screen.game.renderer;

    if (focused) {
        renderer.useColor({1, 1, 1, 1});
        renderer.setTransform({x - 2, y - 2, 0}, {width + 4, height + 4, 1});
        renderer.rect_vbo.draw();
    }

    renderer.useColor({0, 0, 0, 1});
    renderer.setTransform({x, y, 0}, {width, height, 1});
    renderer.rect_vbo.draw();

    const float text_w = renderer.getTextWidth(value, 24);

    if (text_w < (width - text_offset))
        renderer.clearTransform();
    else
        renderer.setTransform({width - text_offset - text_w, 0, 0}, {1, 1, 1});
    renderer.setClip(
        {x, 0, screen.game.window_width, screen.game.window_height});
    renderer.drawText(value, x + text_offset, y + text_offset, 24);
    renderer.setClip(
        {0, 0, screen.game.window_width, screen.game.window_height});
}

void GuiTextInput::onClick(int button, int action, int mods) {
    GuiObject::onClick(button, action, mods);
}

bool GuiTextInput::onKey(int key, int scancode, int action, int mods) {
    if (!focused)
        return false;

    if (key == GLFW_KEY_BACKSPACE && action) {
        if (value.empty())
            return true;

        if (mods & GLFW_MOD_CONTROL) {
            const size_t pos = value.find_last_of(' ');
            if (pos != std::string::npos)
                value.erase(pos);
            else
                value.clear();
        } else {
            value.pop_back();
        }
    } else if (key == GLFW_KEY_ESCAPE) {
        screen.focusObject(nullptr);
        return true;
    }

    return false;
}

void GuiTextInput::onChar(uint codepoint) {
    if (focused &&
        screen.game.renderer.default_font.chars.find(codepoint) !=
        screen.game.renderer.default_font.chars.end()) {
        value += (wchar_t) codepoint;
        if (change_handler)
            change_handler(*this, codepoint);
    }
}
