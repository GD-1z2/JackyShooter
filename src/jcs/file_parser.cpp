#include <jcs/file_parser.hpp>

FileParser::FileParser(const fs::path &file_path) : file{file_path} {}

bool FileParser::next_line() {
    std::getline(file, line);
    line_pos = 0;
    return !file.eof() && !file.fail();
}

int FileParser::read_int() {
    int res{};
    std::stringstream ss{line.substr(line_pos)};
    ss >> res;
    ++line_pos += ss.tellg();
    return res;
}

float FileParser::read_float() {
    float res{};
    std::stringstream ss{line.substr(line_pos)};
    ss >> res;
    ++line_pos += ss.tellg();
    return res;
}

std::string FileParser::read_string(char delimiter) {
    std::string res{};
    std::size_t pos{line_pos};
    while (line[pos] != delimiter) {
        ++pos;
    }
    res = line.substr(line_pos, pos - line_pos);
    line_pos = pos + 1;
    return res;
}

bool FileParser::expect(const std::string &str, bool throw_error) {
    if (line.substr(line_pos, str.size()) != str) {
        if (throw_error)
            throw std::runtime_error{
                "Expected: " + str + " but got: " + line.substr(line_pos)};
        else
            return false;
    }
    line_pos += str.size();

    return true;
}

void FileParser::rollback() {
    line_pos = 0;
}
