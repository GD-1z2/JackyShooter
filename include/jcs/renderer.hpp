#pragma once

#include <jcs/font.hpp>
#include <jcs/model.hpp>
#include <jcs/shader.hpp>
#include <string>
#include "object.hpp"

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
    void set2D() const;

    /**
     * Enables depth testing, sets projection matrix to perspective and view
     * matrix to view.
     */
    void set3D(const glm::mat4 &view) const;

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
    void useTexture(uint texture) const;

    /**
     * @note GUI shader only
     */
    void useColor(const glm::vec4 &color) const;

    /**
     * @note GUI shader only
     */
    void setClip(const glm::vec4 &clip) const;

    /**
     * @note World shader only
     */
    void useMaterial(const Material &material) const;

    void
    drawText(const std::wstring &text, float x, float y, uint size = 0) const;

    float getTextWidth(const std::wstring &text, uint size = 0) const;

    JSGame &game;

    ShaderType current_shader{GUI_SHADER};
    GuiShader gui_shader;
    WorldShader world_shader;

    glm::mat4 proj_3d{}, proj_2d{};
    IndexedVertexBuffer rect_vbo{};
    Model gun_model{*this};
    uint text_vao{}, text_vbo{};
    uint container_texture{}, button_texture{}, steel_texture{};
    BitMapFont default_font;
};
