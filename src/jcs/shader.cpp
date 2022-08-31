#include <glad/glad.hpp>

#include <fstream>
#include <iostream>
#include <jcs/shader.hpp>

Shader::Shader(const char *vertex_path, const char *fragment_path,
               const char *geometry_path) {
    const uint
        vertex{buildShader(vertex_path, GL_VERTEX_SHADER)},
        fragment{buildShader(fragment_path, GL_FRAGMENT_SHADER)},
        geometry{geometry_path != nullptr
                 ? buildShader(geometry_path, GL_GEOMETRY_SHADER)
                 : 0};

    shader_id = glCreateProgram();

    glAttachShader(shader_id, vertex);
    glAttachShader(shader_id, fragment);
    if (nullptr != geometry_path)
        glAttachShader(shader_id, geometry);

    glLinkProgram(shader_id);
    getProgramLinkError(shader_id);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (nullptr != geometry_path)
        glDeleteShader(geometry);
}

void Shader::use() const {
    glUseProgram(shader_id);
}

uint Shader::buildShader(const std::string &path, uint type) {
    std::ifstream file{path};
    std::string source{std::istreambuf_iterator<char>{file},
                       std::istreambuf_iterator<char>{}};
    file.close();

    const char *code{source.c_str()};
    uint shader{glCreateShader(type)};
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    getShaderCompileError(shader);

    return shader;
}

void Shader::getShaderCompileError(uint shader) {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        int log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        if (!log_length) {
            throw std::runtime_error{
                "Shader compilation failed: no logs available"};
        } else {
            char *log = new char[log_length];
            glGetShaderInfoLog(shader, log_length, nullptr, log);
            std::string log_s{log};
            delete[] log;
            throw std::runtime_error{"Shader compilation failed: " + log_s};
        }
    }
}

void Shader::getProgramLinkError(uint program) {
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        int log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

        if (!log_length) {
            throw std::runtime_error{
                "Shader program linking failed: no logs available"};
        } else {
            char *log = new char[log_length];
            glGetProgramInfoLog(program, log_length, nullptr, log);
            std::string log_s{log};
            delete[] log;
            throw std::runtime_error{"Shader program linking failed: " + log_s};
        }
    }
}


GuiShader::GuiShader(const char *vertex_path, const char *fragment_path) :
    Shader{vertex_path, fragment_path},
    model_uniform{glGetUniformLocation(shader_id, "model")},
    view_uniform{glGetUniformLocation(shader_id, "view")},
    projection_uniform{glGetUniformLocation(shader_id, "projection")},
    tc_transform_uniform{glGetUniformLocation(shader_id, "tcTransform")},
    use_texture_uniform{glGetUniformLocation(shader_id, "useTexture")},
    use_color_uniform{glGetUniformLocation(shader_id, "useColor")},
    color_uniform{glGetUniformLocation(shader_id, "color")},
    clip_uniform{glGetUniformLocation(shader_id, "clip")} {
}

void GuiShader::setModel(const glm::mat4 &mat) const {
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, &mat[0][0]);
}

void GuiShader::setView(const glm::mat4 &mat) const {
    glUniformMatrix4fv(view_uniform, 1, GL_FALSE, &mat[0][0]);
}

void GuiShader::setProjection(const glm::mat4 &mat) const {
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, &mat[0][0]);
}

void GuiShader::setTexCoordTransform(const glm::mat4 &mat) const {
    glUniformMatrix4fv(tc_transform_uniform, 1, GL_FALSE, &mat[0][0]);
}

void GuiShader::useTexture(uint texture_id, bool use_color,
                           const glm::vec4 &color) const {
    glUniform1i(use_texture_uniform, 1);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    if (use_color) {
        glUniform1i(use_color_uniform, 1);
        glUniform4fv(color_uniform, 1, &color.r);
    } else
        glUniform1i(use_color_uniform, 0);
}

void GuiShader::useColor(glm::vec4 color) const {
    glUniform1i(use_texture_uniform, 0);
    glUniform4fv(color_uniform, 1, &color.r);
}

void GuiShader::setClip(const glm::vec4 &clip) const {
    glUniform4f(clip_uniform, clip[0], clip[1], clip[2], clip[3]);
}

WorldShader::WorldShader(const char *vertex_path, const char *fragment_path) :
    Shader{vertex_path, fragment_path},
    model_uniform{glGetUniformLocation(shader_id, "model")},
    view_uniform{glGetUniformLocation(shader_id, "view")},
    projection_uniform{glGetUniformLocation(shader_id, "projection")},
    material_specular_exponent_uniform{
        glGetUniformLocation(shader_id, "material.specularExponent")},
    material_use_texture_uniform{
        glGetUniformLocation(shader_id, "material.useTexture")},
    material_color_uniform{glGetUniformLocation(shader_id, "material.color")},
    material_use_metalness_uniform(
        glGetUniformLocation(shader_id, "material.useMetalness")),
    material_metalness_uniform(
        glGetUniformLocation(shader_id, "material.metalnessTexture")),
    view_pos_uniform{glGetUniformLocation(shader_id, "viewPos")},
    light_pos_uniform{glGetUniformLocation(shader_id, "lightPos")} {
}

void WorldShader::setModel(const glm::mat4 &mat) const {
    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, &mat[0][0]);
}

void WorldShader::setView(const glm::mat4 &mat) const {
    glUniformMatrix4fv(view_uniform, 1, GL_FALSE, &mat[0][0]);
}

void WorldShader::setProjection(const glm::mat4 &mat) const {
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, &mat[0][0]);
}

void WorldShader::setMaterial(const Material &mat) const {
    glUniform1f(material_specular_exponent_uniform, mat.specular_exponent);

    glUniform1i(material_use_texture_uniform, mat.use_texture);
    if (mat.use_texture) {
        glBindTexture(GL_TEXTURE_2D, mat.diffuse.texture);
    } else {
        glUniform3f(material_color_uniform,
                    mat.diffuse.color[0],
                    mat.diffuse.color[1],
                    mat.diffuse.color[2]);
    }

    glUniform1i(material_use_metalness_uniform, mat.use_metalness);
    if (mat.use_metalness) {
        glUniform1i(material_metalness_uniform, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mat.metalness_texture);
        glActiveTexture(GL_TEXTURE0);
    }
}

void WorldShader::setViewPos(const glm::vec3 &pos) const {
    glUniform3f(view_pos_uniform, pos.x, pos.y, pos.z);
}

void WorldShader::setLightPos(const glm::vec3 &pos) const {
    glUniform3f(light_pos_uniform, pos.x, pos.y, pos.z);
}

SkyboxShader::SkyboxShader(const char *vertex_path, const char *fragment_path) :
    Shader{vertex_path, fragment_path},
    view_uniform{glGetUniformLocation(shader_id, "view")},
    projection_uniform{glGetUniformLocation(shader_id, "projection")} {
}

void SkyboxShader::setView(const glm::mat4 &mat) const {
    glUniformMatrix4fv(view_uniform, 1, GL_FALSE, &mat[0][0]);
}

void SkyboxShader::setProjection(const glm::mat4 &mat) const {
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, &mat[0][0]);
}
