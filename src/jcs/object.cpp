#include <glad/glad.hpp>

#include <jcs/object.hpp>

VertexBuffer::VertexBuffer(VertexBuffer::Type type,
                           const std::vector<float> *vertices) {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if (vertices)
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<long>(vertices->size() * sizeof(float)),
                     vertices->data(),
                     type == Type::BUF_STATIC ? GL_STATIC_DRAW
                                              : GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) (3 * sizeof(float)));

    if (vertices)
        draw_count = static_cast<int>(vertices->size() / 3);
}

void VertexBuffer::load(const std::vector<float> &vertices) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<long>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    draw_count = static_cast<int>(vertices.size() / 3);
}

void VertexBuffer::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, draw_count);
    glBindVertexArray(0);
}

IndexedVertexBuffer::IndexedVertexBuffer(const std::vector<float> &vertices,
                                         const std::vector<uint> &indices) {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<long>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<long>(indices.size() * sizeof(uint)),
                 indices.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) (3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    draw_count = static_cast<int>(indices.size());
}

void IndexedVertexBuffer::draw() const {
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, draw_count, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
