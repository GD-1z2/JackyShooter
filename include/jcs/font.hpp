#pragma once

#include <glm/vec2.hpp>
#include <jacky-common/defs.hpp>
#include <string>
#include <unordered_map>

struct BitMapFontChar {
    uint id;
    float x, y, width, height, x_advance, x_offset, y_offset;
};

struct BitMapFont {
    BitMapFontChar getChar(u32 code) const;

    BitMapFontChar getCharOrDefault(u32 code) const;

    /**
     * Will write 30 floats to out.
     */
    void writeCharData(float *out,
                       const BitMapFontChar &char_data,
                       float x, float y,
                       float real_size) const;

    /**
     * Will write (text.size() * 30) floats to out.
     */
    void writeStringData(float *out,
                         const std::wstring &text,
                         float x, float y,
                         float real_size) const;

    /**
     * Will write (text.size() * 30) floats to out.
     */
    void writeStringDataRightAligned(float *out,
                                     const std::wstring &text,
                                     float x, float y,
                                     float real_size) const;

    float getTextWidth(const std::wstring &text, float text_size) const;

    std::string name;
    float size;
    bool bold{false}, italic{false}, smooth{true};
    glm::uvec2 spacing;
    uint line_height, base, scaleW, scaleH;
    uint texture; /* OpenGL Texture */

    std::unordered_map<u32, BitMapFontChar> chars;

    BitMapFontChar *default_char{nullptr};
};
