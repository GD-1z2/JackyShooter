#pragma once

#include <jcs/font.hpp>
#include <jcs/model.hpp>
#include <jcs/object.hpp>
#include <jcs/shader.hpp>
#include <jcs/skybox.hpp>
#include <string>

class JSGame;

enum ShaderType {
    GUI_SHADER,
    WORLD_SHADER,
};

class Renderer {
public:
    explicit Renderer(JSGame &game);

    void init();

    void prepareRender();

    /**
     * Disables depth testing, sets projection matrix to orthographic and
     * view matrix to identity.
     */
    void set2D();

    /**
     * Enables depth testing, sets projection matrix to perspective and view
     * matrix to view.
     */
    void set3D(const glm::mat4 &view);

    void useShader(ShaderType type);

    /**
     * Sets model matrix.
     */
    void setTransform(glm::vec3 pos, glm::vec3 scale) const;

    /**
     * Sets model matrix to identity.
     */
    void clearTransform() const;

    /**
     * @note GUI shader only
     */
    void setClip(const glm::vec4 &clip) const;

    /**
     * @note World shader only
     */
    void useMaterial(const Material &material) const;

    void drawText(const std::wstring &text, float x, float y, float size = 0,
                  const glm::vec4 &color = {1, 1, 1, 1}) const;

    float getTextWidth(const std::wstring &text, float size = 0) const;

    JSGame &game;

    ShaderType current_shader{GUI_SHADER};
    GuiShader gui_shader;
    WorldShader world_shader;
    SkyboxShader skybox_shader;

    glm::mat4 proj_3d{}, proj_2d{}, view{};
    IndexedVertexBuffer rect_vbo{};
    Model gun_model{*this};
    uint text_vao{}, text_vbo{};
    uint button_texture{};
    uint skybox_texture{};
    BitMapFont default_font;
};
