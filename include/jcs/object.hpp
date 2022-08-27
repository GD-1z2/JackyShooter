#pragma once

#include <jacky-common/defs.hpp>
#include <vector>

struct VertexBuffer {
    enum Type {
        BUF_STATIC, BUF_DYN
    };

    VertexBuffer() = default;

    explicit VertexBuffer(Type type,
                          const std::vector<float> *vertices = nullptr);

    void load(const std::vector<float> &vertices);

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
