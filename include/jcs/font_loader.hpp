#pragma once

#include <glm/vec2.hpp>
#include <jcs/font.hpp>
#include <string>
#include <variant>
#include <vector>

enum FontLineType {
    FONT_DATA_INFO,
    FONT_DATA_COMMON,
    FONT_DATA_CHAR,
    FONT_DATA_OTHER,
};

enum FontLineDataType {
    FONT_DATA_STR,
    FONT_DATA_INT,
    FONT_DATA_VEC2,
};

struct FontLineData {
    std::string name;
    std::variant<std::string, int, glm::uvec2> value;
};

class FontLoader {
public:
    FontLoader(const std::string &data_path, const std::string &texture_path);

    BitMapFont loadFont();

private:
    FontLineType getLineType();

    std::vector<FontLineData> getLineData();

    const std::string &data_path, &texture_path;
    std::string line;
    uint line_num;
};
