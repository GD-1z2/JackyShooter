#pragma once

#include <glm/vec2.hpp>
#include <jacky-common/types.hpp>
#include <string>
#include <unordered_map>

struct BitMapFontChar {
    uint id, x, y, width, height, x_advance;
    int x_offset, y_offset;
};

struct BitMapFont {
    BitMapFontChar getChar(uint code) const;

    void writeStringData(std::wstring &text, float x, float y, float text_size) const;

    void writeCharData(float *out, const BitMapFontChar &char_data,
                       float x, float y, float text_size) const;

    float getTextWidth(const std::wstring &text, float text_size) const;

    std::string name;
    uint size;
    bool bold{false}, italic{false}, smooth{true};
    glm::uvec2 spacing;
    uint line_height, base, scaleW, scaleH;
    uint texture; /* OpenGL Texture */

    std::unordered_map<uint, BitMapFontChar> chars;
};
