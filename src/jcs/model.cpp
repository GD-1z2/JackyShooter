#include <glad/glad.h>

#include <jcs/model.hpp>
#include <jcs/renderer.hpp>

#define VERTEX_WIDTH (8)
#define VERTEX_SIZE (sizeof(float) * VERTEX_WIDTH)

#define VERTEX_POSITION_OFFSET ((void *) (0 * sizeof(float)))
#define VERTEX_TEXCOORD_OFFSET ((void *) (3 * sizeof(float)))
#define VERTEX_NORMAL_OFFSET ((void *) (5 * sizeof(float)))

#define VERTEX_POSITION_INDEX (0)
#define VERTEX_TEXCOORD_INDEX (1)
#define VERTEX_NORMAL_INDEX (2)

#define VERTEX_POSITION_SIZE (3)
#define VERTEX_TEXCOORD_SIZE (2)
#define VERTEX_NORMAL_SIZE (3)

Material DEFAULT_MAT{
    "DefaultMaterial",
    100, // Specular exponent
    false, // No texture
    {.5, .5, .5}, // Diffuse color
};

void Mesh::init(const std::vector<float> &vertices,
                const std::vector<int> &indices) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<long>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<long>(indices.size() * sizeof(int)),
                 indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(VERTEX_POSITION_INDEX, VERTEX_POSITION_SIZE, GL_FLOAT,
                          GL_FALSE, VERTEX_SIZE, VERTEX_POSITION_OFFSET);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(VERTEX_TEXCOORD_INDEX, VERTEX_TEXCOORD_SIZE, GL_FLOAT,
                          GL_FALSE, VERTEX_SIZE, VERTEX_TEXCOORD_OFFSET);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(VERTEX_NORMAL_INDEX, VERTEX_NORMAL_SIZE, GL_FLOAT,
                          GL_FALSE, VERTEX_SIZE, VERTEX_NORMAL_OFFSET);

    vertex_count = static_cast<int>(indices.size());
}

void Mesh::init(const std::vector<Vertex> &vertices,
                const std::vector<TexCoord> &tex_coords,
                const std::vector<Normal> &normals,
                const std::vector<Face> &faces) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    auto vertex_buffer = std::vector<float>();
    auto index_buffer = std::vector<int>();
    vertex_buffer.reserve(faces.size() * 3 * VERTEX_WIDTH);
    index_buffer.reserve(faces.size() * 3);

    for (const auto &face: faces) {
        for (const auto &vertex: face.vertices) {
            vertex_buffer.push_back(vertices[vertex.vertex_index].x);
            vertex_buffer.push_back(vertices[vertex.vertex_index].y);
            vertex_buffer.push_back(vertices[vertex.vertex_index].z);
            vertex_buffer.push_back(tex_coords[vertex.tex_coord_index].u);
            vertex_buffer.push_back(tex_coords[vertex.tex_coord_index].v);
            vertex_buffer.push_back(normals[vertex.normal_index].x);
            vertex_buffer.push_back(normals[vertex.normal_index].y);
            vertex_buffer.push_back(normals[vertex.normal_index].z);

            index_buffer.push_back(static_cast<int>(index_buffer.size()));
        }
    }

    init(vertex_buffer, index_buffer);
}

void Model::draw() const {
    for (const auto &mesh: meshes) {
        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);

        renderer.useMaterial(*mesh.material);

        glDrawElements(GL_TRIANGLES, mesh.vertex_count, GL_UNSIGNED_INT,
                       nullptr);
    }
}
