#include <glad/glad.hpp>

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

    ModelLoader{"assets/models/player/player.obj", player_model}.load();
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
    const auto model = glm::scale(glm::translate(glm::mat4{1.f}, pos), scale);
    if (GUI_SHADER == current_shader)
        gui_shader.setModel(model);
    else
        world_shader.setModel(model);
}

void
Renderer::setTransform(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot) const {
    auto model = glm::translate(glm::mat4{1.f}, pos);
    model = glm::rotate(model, glm::radians(rot.x), glm::vec3{1, 0, 0});
    model = glm::rotate(model, glm::radians(rot.y), glm::vec3{0, 1, 0});
    model = glm::rotate(model, glm::radians(rot.z), glm::vec3{0, 0, 1});
    model = glm::scale(model, scale);

    if (GUI_SHADER == current_shader)
        gui_shader.setModel(model);
    else
        world_shader.setModel(model);
}

void Renderer::clearTransform() const {
    if (GUI_SHADER == current_shader)
        gui_shader.setModel(glm::mat4{1.f});
    else
        world_shader.setModel(glm::mat4{1.f});
}

void Renderer::setClip(const glm::vec4 &clip) const {
    gui_shader.setClip(clip);
}

void Renderer::useMaterial(const Material &material) const {
    world_shader.setMaterial(material);
}

void Renderer::drawText(const std::wstring &text,
                        float x, float y,
                        float size,
                        const glm::vec4 &color) const {
    if (0 == size) size = default_font.size;

    gui_shader.useTexture(default_font.texture, true, color);

    if (GUI_SHADER == current_shader)
        gui_shader.setTexCoordTransform(glm::mat4{1});

    glBindVertexArray(text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo);

    const auto buff = new float[text.length() * 30];
    default_font.writeStringData(buff, text, x, y, size);

    glBufferData(GL_ARRAY_BUFFER, (long) (text.length() * 30 * sizeof(float)),
                 buff, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, (int) text.length() * 6);

    delete[] buff;
}

float Renderer::getTextWidth(const std::wstring &text, float size) const {
    const float text_size = (0 == size) ? 1 : size / default_font.size;

    float width{};

    for (int i{}; i < text.length(); i++)
        width += default_font.getChar(text[i]).x_advance * text_size;

    return width;
}
