#include <glad/glad.h>
#include <jcs/font.hpp>

BitMapFontChar BitMapFont::getChar(uint code) const {
    return chars.at(code);
}

void BitMapFont::writeStringData(std::wstring &text, float x, float y,
                                 float text_size) const {
    text_size /= (float) size;

    const auto buff = new float[text.length() * 30];

    for (size_t i{}; i < text.length(); i++) {
        const auto char_data = getChar(text[i]);
        writeCharData(buff + i * 30, char_data, x, y, text_size);
        x += (float) char_data.x_advance * text_size;
    }

    glBufferData(GL_ARRAY_BUFFER, (long) (text.length() * 30 * sizeof(float)),
                 buff, GL_DYNAMIC_DRAW);

    delete[] buff;
}

void BitMapFont::writeCharData(float *out, const BitMapFontChar &char_data,
                               float x, float y, float text_size) const {
    text_size /= (float) size;

    const float
        x_ = x + (float) char_data.x_offset * text_size,
        y_ = y + (float) char_data.y_offset * text_size,
        x_end = x_ + (float) char_data.width * text_size,
        y_end = y_ + (float) char_data.height * text_size,
        tex_x = (float) char_data.x / 512.f,
        tex_x_end = (float) (char_data.x + char_data.width) / 512.f,
        tex_y = (float) char_data.y / -256.f,
        tex_y_end = (float) (char_data.y + char_data.height) / -256.f;

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

float BitMapFont::getTextWidth(const std::wstring &text,
                               float text_size) const {
    text_size /= (float) size;

    float width{};

    for (const wchar_t c: text)
        width += (float) getChar(c).x_advance * text_size;

    return width;
}
