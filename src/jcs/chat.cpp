#include <glad/glad.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <jcs/chat.hpp>
#include <jcs/game.hpp>
#include <jcs/game_screen.hpp>

Chat::Chat(JSGame &game) : game{game} {}

void Chat::initRender() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Chat::addMessage(const std::wstring &message) {
    messages.push_front(message);
    if (messages.size() > 6)
        messages.pop_back();

    animation_start = game.now;

    updateBuffer();
}

void Chat::updateBuffer() {
    const float
        real_size = chat_screen_opened ? 16.f : 12.f,
        text_size = real_size / game.renderer.default_font.size,
        max_width = chat_screen_opened ? game.window_width
                                       : std::max(200.f,
                                                  game.window_width / 5);

    chat_height = 0;

    animation_height = 1;

    int x, y{}, i{0};
    std::vector<float> buff;

    for (auto it = messages.rbegin(); it < messages.rend(); it++) {
        const auto &msg = *it;
        x = 0;

        buff.resize((i + msg.length()) * 30);

        for (int j{0}; j < msg.length(); j++, i++) {
            const auto char_data = game.renderer.default_font.getChar(msg[j]);

            if ((float) x + (char_data.width + char_data.x_offset) * text_size
                > max_width) {
                x = 0;
                y += (int) ((float) game.renderer.default_font.line_height *
                            text_size);

                chat_height += (int) (
                    (float) game.renderer.default_font.line_height *
                    text_size);

                if (1 == std::distance(it, messages.rend()))
                    animation_height++;
            }

            game.renderer.default_font.writeCharData(
                buff.data() + i * 30,
                char_data,
                (float) x, (float) y,
                real_size);

            x += (int) (char_data.x_advance * text_size);
        }

        y += (int) ((float) game.renderer.default_font.line_height * text_size
                    * 1.1);
        chat_height += (uint) ((float) game.renderer.default_font.line_height *
                               text_size
                               * 1.1);
    }

    animation_height *=
        (float) game.renderer.default_font.line_height * text_size;

    triangle_count = (int) buff.size() / 5;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (long) (buff.size() * sizeof(float)),
                 buff.data(), GL_DYNAMIC_DRAW);
}

void Chat::render() const {
    float anim_offset =
        (1.f -
         std::min(1.f, 4 * (game.now - animation_start)))
        * animation_height;
    if (chat_screen_opened)
        anim_offset -= 50;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindTexture(GL_TEXTURE_2D, game.renderer.default_font.texture);

    game.renderer.setTransform(
        {10, game.window_height - chat_height + anim_offset, 0}, {1, 1, 1});
    game.renderer.gui_shader.useTexture(
        game.renderer.default_font.texture, true, glm::vec4{1, 1, 1, .5});

    glDrawArrays(GL_TRIANGLES, 0, triangle_count);

    game.renderer.gui_shader.setProjection(game.renderer.proj_2d);
}

ChatScreen::ChatScreen(JSGame &game, Chat &chat) :
    Screen{game},
    parent_screen{dynamic_cast<GameScreen &>(game.getScreen())},
    chat{chat},
    textInput{
        *this,
        5, game.window_height - 45,
        game.window_width - 10, 40,
        [](GuiObject &o) {
            o.y = o.screen.game.window_height - 45;
            o.width = o.screen.game.window_width - 10;
        },
        nullptr
    } {
    parent_screen.display_overlays = false;
    chat.chat_screen_opened = true;
    chat.updateBuffer();
    gui_objects.push_back(&textInput);
    focusObject(&textInput);
}

ChatScreen::~ChatScreen() {
    parent_screen.display_overlays = true;
    chat.chat_screen_opened = false;
    chat.updateBuffer();
}

void ChatScreen::update() {
    if (!can_type && GLFW_RELEASE == game.inputs.getInput(JSINPUT_CHAT))
        can_type = true;
}

void ChatScreen::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    parent_screen.render();

    game.renderer.set2D();

    game.renderer.gui_shader.useColor({0, 0, 0, .3});
    game.renderer.setTransform({0, 0, 0},
                               {game.window_width, game.window_height, 1});
    game.renderer.rect_vbo.draw();

    game.renderer.clearTransform();

    game.renderer.gui_shader.useTexture(game.renderer.default_font.texture);
    chat.render();

    Screen::render();
}

void ChatScreen::onResize() {
    Screen::onResize();
    chat.updateBuffer();
}

void ChatScreen::onFocus() {
    glfwSetInputMode(game.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool ChatScreen::onKey(int key, int scancode, int action, int mods) {
    if (game.inputs.is(key, JSINPUT_ESCAPE) && GLFW_PRESS == action) {
        game.removeTopScreen();
        return true;
    }

    if (GLFW_KEY_ENTER == key && GLFW_PRESS == action) {
        if (textInput.value.length()) {
            chat.addMessage(textInput.value);
            textInput.value = L"";
            return true;
        } else {
            game.removeTopScreen();
            return true;
        }
    }

    if (Screen::onKey(key, scancode, action, mods))
        return true;

    return false;
}

void ChatScreen::onChar(uint codepoint) {
    if (can_type)
        Screen::onChar(codepoint);
}
