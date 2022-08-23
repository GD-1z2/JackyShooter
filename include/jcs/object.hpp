#pragma once

#include <jacky-common/defs.hpp>
#include <vector>

struct VertexBuffer {
    VertexBuffer() = default;

    explicit VertexBuffer(const std::vector<float> &vertices);

    void draw() const;

    uint vbo{}, vao{};
    int draw_count{};
};

struct IndexedVertexBuffer {
    IndexedVertexBuffer() = default;

    IndexedVertexBuffer(const std::vector<float> &vertices,
                        const std::vector<uint> &indices);

    void draw() const;

    uint vbo{}, vao{}, ebo{};
    int draw_count{};
};
