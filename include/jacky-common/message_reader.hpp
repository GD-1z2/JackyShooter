#pragma once

#include <jacky-common/types.hpp>
#include <stdexcept>
#include <string>
#include <vector>

class MessageReader {
public:
    explicit MessageReader(const std::string &message) : message{message} {}

    u16 readUInt16() {
        if (pos + 2 > message.size())
            throw std::runtime_error{"Incomplete message"};

        const u16 val = message[pos] << 8 | message[pos + 1];
        pos += 2;
        return val;
    }

    template<typename T>
    T readArr(int len) {
        const int size = (len * sizeof(typename T::value_type));

        if (pos + size > message.size())
            throw std::runtime_error{"Incomplete message"};

        T arr;
        arr.resize(len);
        std::copy(
            message.begin() + (int) pos,
            message.begin() + (int) pos + len,
            arr.begin()
        );

        pos += len;

        return arr;
    }

private:
    const std::string &message;
    size_t pos{};
};
