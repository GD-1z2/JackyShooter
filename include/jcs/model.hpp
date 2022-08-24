#pragma once

#include <jacky-common/types.hpp>
#include <vector>
#include <string>

struct Vertex {
    float x, y, z;
};

struct TexCoord {
    float u, v;
};

struct Normal {
    float x, y, z;
};

struct Face {
    struct FaceVertex {
        bool operator==(const FaceVertex &rhs) const {
            return vertex_index == rhs.vertex_index &&
                   tex_coord_index == rhs.tex_coord_index &&
                   normal_index == rhs.normal_index;
        }

        int vertex_index, tex_coord_index, normal_index;
    } vertices[3];
};

struct Material {
    std::string name;
    float specular_exponent;

    bool use_texture;
    union {
        float color[3];
        uint texture;
    } diffuse;

    bool use_metalness;
    uint metalness_texture;
};

extern Material DEFAULT_MAT;

struct Mesh {
    void init(const std::vector<float> &vertices,
              const std::vector<int> &indices);

    void init(const std::vector<Vertex> &vertices,
              const std::vector<TexCoord> &tex_coords,
              const std::vector<Normal> &normals,
              const std::vector<Face> &faces);

    const Material *material{&DEFAULT_MAT};
    uint vao{}, vbo{}, ebo{};
    int vertex_count{};
};

class Renderer;

struct Model {
    Model() = delete;

    void draw() const;

    Renderer &renderer;
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
};
