#pragma once

#include <cstring>
#include <jacky-common/types.hpp>
#include <stdexcept>
#include <string>
#include <vector>

class MessageReader {
public:
    explicit MessageReader(const std::string &message) : message{message} {}

    u8 readByte() {
        if (pos >= message.size()) {
            throw std::runtime_error("MessageReader: readByte: end of message");
        }
        return message[pos++];
    }

    u16 readShort() {
        if (pos + 2 > message.size()) {
            throw std::runtime_error(
                "MessageReader: readShort: end of message");
        }
        const u16 val = (message[pos] << 8) | message[pos + 1];
        pos += 2;
        return val;
    }

    u32 readInt() {
        if (pos + 4 > message.size()) {
            throw std::runtime_error("MessageReader: readInt: end of message");
        }
        const u32 val = (message[pos] << 24) | (message[pos + 1] << 16) |
                        (message[pos + 2] << 8) | message[pos + 3];
        pos += 4;
        return val;
    }

    template<typename T>
    T readArr() {
        const uint
            size = readInt(),
            len = (size / sizeof(typename T::value_type));

        if (pos + size > message.size())
            throw std::runtime_error{"Incomplete message"};

        T arr;
        arr.resize(len);

        std::memcpy(arr.data(), message.data() + pos, size);

        pos += size;

        return arr;
    }

private:
    const std::string &message;
    size_t pos{};
};
