#include <fstream>
#include <jcs/font_loader.hpp>
#include <jcs/texture.hpp>
#include <regex>
#include <iostream>

FontLoader::FontLoader(const std::string &data_path, const std::string &texture_path) :
    data_path{data_path},
    texture_path{texture_path},
    line_num{} {}

BitMapFont FontLoader::loadFont() {
    BitMapFont font{};
    font.texture = loadTexture(texture_path);

    std::ifstream file{data_path};

    for (; std::getline(file, line); line_num++) {
        if (line.empty())
            continue;

        const auto line_type = getLineType();
        const auto line_data = getLineData();

        if (line_type == FONT_DATA_INFO) {
            for (const auto &[name, val]: line_data)
                if (name == "face" && val.index() == FONT_DATA_STR)
                    font.name = std::get<std::string>(val);
                else if (name == "size" && val.index() == FONT_DATA_INT)
                    font.size = std::get<int>(val);
                else if (name == "bold" && val.index() == FONT_DATA_INT)
                    font.bold = std::get<int>(val);
                else if (name == "italic" && val.index() == FONT_DATA_INT)
                    font.italic = std::get<int>(val);
                else if (name == "smooth" && val.index() == FONT_DATA_INT)
                    font.smooth = std::get<int>(val);
                else if (name == "spacing" && val.index() == FONT_DATA_VEC2)
                    font.spacing = std::get<glm::uvec2>(val);
        } else if (line_type == FONT_DATA_COMMON) {
            for (const auto &[name, val]: line_data)
                if (name == "lineHeight" && val.index() == FONT_DATA_INT)
                    font.line_height = std::get<int>(val);
                else if (name == "base" && val.index() == FONT_DATA_INT)
                    font.base = std::get<int>(val);
                else if (name == "scaleW" && val.index() == FONT_DATA_INT)
                    font.scaleW = std::get<int>(val);
                else if (name == "scaleH" && val.index() == FONT_DATA_INT)
                    font.scaleH = std::get<int>(val);
                else if (name == "pages" && val.index() == FONT_DATA_INT && std::get<int>(val) != 1)
                    throw std::runtime_error{"Only one font texture page is supported"};
        } else if (line_type == FONT_DATA_CHAR) {
            BitMapFontChar ch{};

            for (const auto &[name, val]: line_data)
                if (name == "id" && val.index() == FONT_DATA_INT)
                    ch.id = std::get<int>(val);
                else if (name == "x" && val.index() == FONT_DATA_INT)
                    ch.x = std::get<int>(val);
                else if (name == "y" && val.index() == FONT_DATA_INT)
                    ch.y = std::get<int>(val);
                else if (name == "width" && val.index() == FONT_DATA_INT)
                    ch.width = std::get<int>(val);
                else if (name == "height" && val.index() == FONT_DATA_INT)
                    ch.height = std::get<int>(val);
                else if (name == "xoffset" && val.index() == FONT_DATA_INT)
                    ch.x_offset = std::get<int>(val);
                else if (name == "yoffset" && val.index() == FONT_DATA_INT)
                    ch.y_offset = std::get<int>(val);
                else if (name == "xadvance" && val.index() == FONT_DATA_INT)
                    ch.x_advance = std::get<int>(val);

            font.chars[ch.id] = ch;

            static const u32 QUESTION_MARK_CODE = 0x3F;
            if (QUESTION_MARK_CODE == ch.id)
                font.default_char = &font.chars[ch.id];
        }
    }

    file.close();

    return font;
}

FontLineType FontLoader::getLineType() {
    static const std::regex line_type_regex{R"(^(\w+)\s)"};

    std::smatch match;

    if (!std::regex_search(line, match, line_type_regex) || match.size() != 2)
        throw std::runtime_error{"Invalid line " + std::to_string(line_num)};

    const std::string line_type = match[1];

    if (line_type == "info")
        return FONT_DATA_INFO;
    else if (line_type == "common")
        return FONT_DATA_COMMON;
    else if (line_type == "char")
        return FONT_DATA_CHAR;

    else if (line_type == "page")
        return FONT_DATA_OTHER;
    else
        throw std::runtime_error{"Invalid/unsupported line type \"" + line_type + "\""};
}

std::vector<FontLineData> FontLoader::getLineData() {
    static const std::regex params_regex{R"((\w+)\s*=\s*("(.*)\"|((\d+),(\d+))|(-?\d+)))"};

    std::vector<FontLineData> data;

    for (std::sregex_iterator it{line.cbegin(), line.cend(), params_regex}; it != std::sregex_iterator{}; it++) {
        const auto
            &name{(*it)[1]},
            &str_val{(*it)[3]},
            &vec_val{(*it)[4]}, &vec_x_val{(*it)[5]}, &vec_y_val{(*it)[6]},
            &int_val{(*it)[7]};

        if (str_val.matched)
            data.push_back({name, str_val});
        else if (int_val.matched)
            data.push_back({name, std::stoi(int_val)});
        else if (vec_val.matched)
            data.push_back({name, glm::uvec2{std::stoul(vec_x_val), std::stoul(vec_y_val)}});
        else
            throw std::runtime_error{"Invalid/unsupported parameter type \"" + name.str() + "\""};
    }

    return data;
}
