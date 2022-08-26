#include <glad/glad.hpp>

#include <jcs/font.hpp>

#include <iostream>

BitMapFontChar BitMapFont::getChar(uint code) const {
    return chars.at(code);
}

BitMapFontChar BitMapFont::getCharOrDefault(u32 code) const {
    if (!default_char)
        return chars.at(code);

    const auto it = chars.find(code);
    if (chars.end() == it)
        return *default_char;
    return it->second;
}

void BitMapFont::writeCharData(float *out, const BitMapFontChar &char_data,
                               float x, float y, float real_size) const {
    const float
        text_size = real_size / size,
        x_ = x + char_data.x_offset * text_size,
        y_ = y + char_data.y_offset * text_size,
        x_end = x_ + char_data.width * text_size,
        y_end = y_ + char_data.height * text_size,
        tex_x = char_data.x / 512.f,
        tex_x_end = (char_data.x + char_data.width) / 512.f,
        tex_y = char_data.y / -256.f,
        tex_y_end = (char_data.y + char_data.height) / -256.f;

    out[0] = x_end;
    out[5] = x_end;
    out[10] = x_;
    out[15] = x_end;
    out[20] = x_;
    out[25] = x_;

    out[1] = y_end;
    out[6] = y_;
    out[11] = y_end;
    out[16] = y_;
    out[21] = y_;
    out[26] = y_end;

    out[2] = 0;
    out[7] = 0;
    out[12] = 0;
    out[17] = 0;
    out[22] = 0;
    out[27] = 0;

    out[3] = tex_x_end;
    out[8] = tex_x_end;
    out[13] = tex_x;
    out[18] = tex_x_end;
    out[23] = tex_x;
    out[28] = tex_x;

    out[4] = tex_y_end;
    out[9] = tex_y;
    out[14] = tex_y_end;
    out[19] = tex_y;
    out[24] = tex_y;
    out[29] = tex_y_end;
}

void BitMapFont::writeStringData(float *out,
                                 const std::wstring &text,
                                 float x, float y,
                                 float real_size) const {
    for (size_t i{}; i < text.length(); i++) {
        const auto char_data = getCharOrDefault(text[i]);
        writeCharData(out + i * 30, char_data, x, y, real_size);
        x += char_data.x_advance * (real_size / size);
    }
}

void BitMapFont::writeStringDataRightAligned(float *out,
                                             const std::wstring &text,
                                             float x, float y,
                                             float real_size) const {
    const float text_size = real_size / size;
    for (size_t i{text.length()}; i > 0; i--) {
        const auto char_data = getCharOrDefault(text[i - 1]);
        writeCharData(out + (i - 1) * 30, char_data, x - char_data.x_advance * text_size, y, real_size);
        x -= char_data.x_advance * text_size;
    }
}

float BitMapFont::getTextWidth(const std::wstring &text,
                               float text_size) const {
    text_size /= (float) size;

    float width{};

    for (const wchar_t c: text)
        width += (float) getChar(c).x_advance * text_size;

    return width;
}
