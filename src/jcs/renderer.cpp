#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <jcs/font_loader.hpp>
#include <jcs/game.hpp>
#include <jcs/model_loader.hpp>
#include <jcs/renderer.hpp>
#include <jcs/texture.hpp>

static const std::vector<float> RECTANGLE_VERTICES{
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
};

static const std::vector<uint> RECTANGLE_INDICES{0, 2, 1, /**/ 1, 2, 3};

Renderer::Renderer(JSGame &game) : game{game} {}

void Renderer::init() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    gui_shader = {"assets/shaders/gui.vert", "assets/shaders/gui.frag"};
    world_shader = {"assets/shaders/world.vert", "assets/shaders/world.frag"};
    skybox_shader = {"assets/shaders/skybox.vert",
                     "assets/shaders/skybox.frag"};

    rect_vbo = {RECTANGLE_VERTICES, RECTANGLE_INDICES};

    ModelLoader{"assets/models/gun0/gun0.obj", gun_model}.load();

    glGenVertexArrays(1, &text_vao);
    glBindVertexArray(text_vao);

    glGenBuffers(1, &text_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo);

    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) (0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    button_texture = loadTexture("assets/textures/button.png", false);

    skybox_texture = loadCubeMap(
        {
            "assets/textures/skybox/left.png",
            "assets/textures/skybox/right.png",
            "assets/textures/skybox/top.png",
            "assets/textures/skybox/bottom.png",
            "assets/textures/skybox/front.png",
            "assets/textures/skybox/back.png",
        }
    );

    default_font = FontLoader{
        "assets/fonts/open_sans_regular_24.fnt",
        "assets/fonts/open_sans_regular_24.png",
    }.loadFont();
}

void Renderer::prepareRender() {
    gui_shader.use();
    gui_shader.setClip({0, 0, game.window_width, game.window_height});
    proj_3d = glm::perspective(glm::radians(45.f),
                               game.window_width / game.window_height, .1f,
                               100.f);
    proj_2d = glm::ortho(0.f, game.window_width, game.window_height, 0.f, -1.f,
                         1.f);
}

void Renderer::set2D() {
    glDisable(GL_DEPTH_TEST);

    view = glm::mat4{1};

    if (GUI_SHADER == current_shader) {
        gui_shader.setProjection(proj_2d);
        gui_shader.setView(view);
    } else {
        world_shader.setProjection(proj_2d);
        world_shader.setView(view);
    }
}

void Renderer::set3D(const glm::mat4 &view_) {
    glEnable(GL_DEPTH_TEST);

    this->view = view_;

    if (GUI_SHADER == current_shader) {
        gui_shader.setProjection(proj_3d);
        gui_shader.setView(view);
    } else {
        world_shader.setProjection(proj_3d);
        world_shader.setView(view);
    }
}

void Renderer::useShader(ShaderType type) {
    current_shader = type;

    switch (type) {
        case GUI_SHADER:
            gui_shader.use();
            break;
        case WORLD_SHADER:
            world_shader.use();
            break;
    }
}

void Renderer::setTransform(glm::vec3 pos, glm::vec3 scale) const {
    if (GUI_SHADER == current_shader)
        gui_shader.setModel(
            glm::scale(glm::translate(glm::mat4{1.f}, pos), scale));
    else
        world_shader.setModel(
            glm::scale(glm::translate(glm::mat4{1.f}, pos), scale));
}

void Renderer::clearTransform() const {
    if (GUI_SHADER == current_shader)
        gui_shader.setModel(glm::mat4{1.f});
    else
        world_shader.setModel(glm::mat4{1.f});
}

void Renderer::useTexture(uint texture) const {
    glBindTexture(GL_TEXTURE_2D, texture);
    gui_shader.setColor(false);
}

void Renderer::useColor(const glm::vec4 &color) const {
    gui_shader.setColor(true, color);
}

void Renderer::setClip(const glm::vec4 &clip) const {
    gui_shader.setClip(clip);
}

void Renderer::useMaterial(const Material &material) const {
    world_shader.setMaterial(material);
}

void Renderer::drawText(const std::wstring &text, float x, float y,
                        uint size) const {
    if (!size) size = default_font.size;

    useTexture(default_font.texture);

    if (GUI_SHADER == current_shader)
        gui_shader.setTexCoordTransform(glm::mat4{1});

    glBindVertexArray(text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo);

    const auto buff = new float[text.length() * 30];

    for (int i{}; i < text.length(); i++) {
        const auto char_data = default_font.getChar(text[i]);

        const float
            text_size = (float) size / (float) default_font.size,
            x_ = x + (float) char_data.x_offset * text_size,
            y_ = y + (float) char_data.y_offset * text_size,
            x_end = x_ + (float) char_data.width * text_size,
            y_end = y_ + (float) char_data.height * text_size,
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

        x += (float) char_data.x_advance * text_size;
    }

    glBufferData(GL_ARRAY_BUFFER, (long) (text.length() * 30 * sizeof(float)),
                 buff, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, (int) text.length() * 6);

    delete[] buff;
}

float Renderer::getTextWidth(const std::wstring &text, uint size) const {
    float width{};

    const float text_size = size ? (float) size / (float) default_font.size : 1;

    for (int i{}; i < text.length(); i++)
        width += (float) default_font.getChar(text[i]).x_advance * text_size;

    return width;
}
