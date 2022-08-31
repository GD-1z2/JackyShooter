#pragma once

#include <glm/glm.hpp>
#include <jacky-common/defs.hpp>
#include <jcs/model.hpp>
#include <string>

struct Shader {
    Shader() = default;

    Shader(const char *vertex_path, const char *fragment_path,
           const char *geometry_path = nullptr);

    void use() const;

    uint shader_id{};

private:
    static uint buildShader(const std::string &path, uint type);

    static void getShaderCompileError(uint shader);

    static void getProgramLinkError(uint program);
};

struct GuiShader : public Shader {
    GuiShader() = default;

    GuiShader(const char *vertex_path, const char *fragment_path);

    void setModel(const glm::mat4 &mat) const;

    void setView(const glm::mat4 &mat) const;

    void setProjection(const glm::mat4 &mat) const;

    void setTexCoordTransform(const glm::mat4 &mat) const;

    void useTexture(uint texture_id,
                    bool use_color = false,
                    const glm::vec4 &color = {}) const;

    void useColor(glm::vec4 color = {1, 1, 1, 1}) const;

    void setClip(const glm::vec4 &clip) const;

    int
        model_uniform{},
        view_uniform{},
        projection_uniform{},
        tc_transform_uniform{},
        use_texture_uniform{},
        use_color_uniform{},
        color_uniform{},
        clip_uniform{};
};

struct WorldShader : public Shader {
    WorldShader() = default;

    WorldShader(const char *vertex_path, const char *fragment_path);

    void setModel(const glm::mat4 &mat) const;

    void setView(const glm::mat4 &mat) const;

    void setProjection(const glm::mat4 &mat) const;

    void setMaterial(const Material &material) const;

    void setViewPos(const glm::vec3 &pos) const;

    void setLightPos(const glm::vec3 &pos) const;

    int
        model_uniform{},
        view_uniform{},
        projection_uniform{},
        material_specular_exponent_uniform{},
        material_use_texture_uniform{},
        material_color_uniform{},
        material_use_metalness_uniform{},
        material_metalness_uniform{},
        view_pos_uniform{},
        light_pos_uniform{};
};

struct SkyboxShader : public Shader {
    SkyboxShader() = default;

    SkyboxShader(const char *vertex_path, const char *fragment_path);

    void setView(const glm::mat4 &mat) const;

    void setProjection(const glm::mat4 &mat) const;

    int view_uniform{}, projection_uniform{};
};
