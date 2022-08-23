#pragma once

#include <fstream>
#include <jacky-common/defs.hpp>

class FileParser {
public:
    explicit FileParser(const fs::path &file_path);

    /**
     * @brief Reads the next line from the file.
     * @return True if a line was read, false if the end of the file was reached
     * or an error occurred.
     */
    bool next_line();

    int read_int();

    float read_float();

    std::string read_string(char delimiter = '\n');

    bool expect(const std::string &str, bool throw_error = false);

    void rollback();

private:
    std::ifstream file;
    std::string line{};
    std::size_t line_pos{};
};
