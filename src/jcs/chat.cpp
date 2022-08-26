#include "glad/glad.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <jcs/chat.hpp>
#include <jcs/game.hpp>

Chat::Chat(JSGame &game) :
    game{game},
    vao{0}, vbo{0}, triangle_count{0},
    max_width{200},
    animation_height{0}, animation_start{0.f} {}

void Chat::initRender() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Chat::addMessage(const std::wstring &message) {
    messages.push_front(message);
    if (messages.size() > 6)
        messages.pop_back();

    const float text_size = 12 / game.renderer.default_font.size;

    chat_height = 0;

    animation_start = game.now;
    animation_height = 1;

    int x, y{}, i{0};
    std::vector<float> buff;

    for (auto it = messages.rbegin(); it < messages.rend(); it++) {
        const auto &msg = *it;
        x = 0;

        buff.resize((i + msg.length()) * 30);

        for (int j{0}; j < msg.length(); j++, i++) {
            const auto char_data = game.renderer.default_font.getChar(msg[j]);

            if ((uint) ((float) x +
                        (char_data.width + char_data.x_offset) * text_size
                        ) > max_width) {
                x = 0;
                y += (int) ((float) game.renderer.default_font.line_height *
                            text_size);

                chat_height++;

                if (1 == std::distance(it, messages.rend()))
                    animation_height++;
            }

            game.renderer.default_font.writeCharData(
                buff.data() + i * 30,
                char_data, (float) x,
                (float) y, 12);

            x += (int) (char_data.x_advance * text_size);
        }

        y += (int) ((float) game.renderer.default_font.line_height * text_size);
        chat_height++;
    }

    chat_height = (uint) ((float) chat_height *
                          (float) game.renderer.default_font.line_height *
                          text_size);
    animation_height *=
        (float) game.renderer.default_font.line_height * text_size;

    triangle_count = (int) buff.size() / 5;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (long) (buff.size() * sizeof(float)),
                 buff.data(), GL_DYNAMIC_DRAW);
}

void Chat::render() const {
    const int anim_offset = (int) (
        (1.f - std::min((game.now - animation_start) * 4.f, 1.f)) *
        animation_height);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindTexture(GL_TEXTURE_2D, game.renderer.default_font.texture);

    game.renderer.gui_shader.setProjection(
        glm::translate(
            game.renderer.proj_2d,
            {10, game.window_height - chat_height + anim_offset, 0}
        )
    );

    glDrawArrays(GL_TRIANGLES, 0, triangle_count);

    game.renderer.gui_shader.setProjection(game.renderer.proj_2d);
}
