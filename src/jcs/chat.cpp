#include <glad/glad.h>

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Chat::addMessage(const std::wstring &message) {
    messages.push_front(message);
    if (messages.size() > 6)
        messages.pop_back();

    const float text_size = .5f;

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

            if ((uint) ((float) x + (float) (char_data.width + char_data.x_offset) * text_size) > max_width) {
                x = 0;
                y += (int) ((float) game.renderer.default_font.line_height * text_size);

                chat_height++;

                if (std::distance(it, messages.rend()) == 1)
                    animation_height++;
            }

            const auto
                x_ = (float) x + (float) char_data.x_offset * text_size,
                y_ = (float) y + (float) char_data.y_offset * text_size,
                x_end = (float) x + (float) (char_data.width + char_data.x_offset) * text_size,
                y_end = (float) y + (float) (char_data.height + char_data.y_offset) * text_size,
                tex_x = (float) char_data.x / 512.f,
                tex_x_end = (float) (char_data.x + char_data.width) / 512.f,
                tex_y = (float) char_data.y / -256.f,
                tex_y_end = (float) (char_data.y + char_data.height) / -256.f;

            buff[i * 30 + 0] = x_end;
            buff[i * 30 + 5] = x_end;
            buff[i * 30 + 10] = x_;
            buff[i * 30 + 15] = x_end;
            buff[i * 30 + 20] = x_;
            buff[i * 30 + 25] = x_;

            buff[i * 30 + 1] = y_end;
            buff[i * 30 + 6] = y_;
            buff[i * 30 + 11] = y_end;
            buff[i * 30 + 16] = y_;
            buff[i * 30 + 21] = y_;
            buff[i * 30 + 26] = y_end;

            buff[i * 30 + 2] = 0;
            buff[i * 30 + 7] = 0;
            buff[i * 30 + 12] = 0;
            buff[i * 30 + 17] = 0;
            buff[i * 30 + 22] = 0;
            buff[i * 30 + 27] = 0;

            buff[i * 30 + 3] = tex_x_end;
            buff[i * 30 + 8] = tex_x_end;
            buff[i * 30 + 13] = tex_x;
            buff[i * 30 + 18] = tex_x_end;
            buff[i * 30 + 23] = tex_x;
            buff[i * 30 + 28] = tex_x;

            buff[i * 30 + 4] = tex_y_end;
            buff[i * 30 + 9] = tex_y;
            buff[i * 30 + 14] = tex_y_end;
            buff[i * 30 + 19] = tex_y;
            buff[i * 30 + 24] = tex_y;
            buff[i * 30 + 29] = tex_y_end;

            x += (int) ((float) char_data.x_advance * text_size);
        }

        y += (int) ((float) game.renderer.default_font.line_height * text_size);
        chat_height++;
    }

    chat_height = (uint) ((float) chat_height * (float) game.renderer.default_font.line_height * text_size);
    animation_height *= (float) game.renderer.default_font.line_height * text_size;

    triangle_count = (int) buff.size() / 5;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (long) (buff.size() * sizeof(float)), buff.data(), GL_DYNAMIC_DRAW);
}

void Chat::render() const {
    const int anim_offset = (int) ((1.f - std::min((game.now - animation_start) * 4.f, 1.f)) * animation_height);

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
