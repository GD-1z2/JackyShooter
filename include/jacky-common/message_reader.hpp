#pragma once

#include <cstring>
#include <jacky-common/defs.hpp>
#include <stdexcept>
#include <string>
#include <vector>

class MessageReader {
public:
    explicit MessageReader(const std::string &message) :
        data{(u8 *) message.data()}, msg_size{message.size()} {}

    u8 readByte() {
        if (pos >= msg_size)
            throw std::runtime_error("MessageReader: readByte: end of message");

        return data[pos++];
    }

    u16 readShort() {
        if (pos + 2 > msg_size)
            throw std::runtime_error(
                "MessageReader: readShort: end of message");

        const u16 val = (data[pos] << 8) | data[pos + 1];
        pos += 2;
        return val;
    }

    u32 readInt() {
        if (pos + 4 > msg_size)
            throw std::runtime_error("MessageReader: readInt: end of message");

        const u32 val = (data[pos] << 24) | (data[pos + 1] << 16) |
                        (data[pos + 2] << 8) | data[pos + 3];
        pos += 4;
        return val;
    }

    float readFloat() {
        if (pos + 4 > msg_size)
            throw std::runtime_error(
                "MessageReader: readFloat: end of message");

        const u32 i = readInt();
        return *reinterpret_cast<const float *>(&i);
    }

    template<typename T>
    T readArr() {
        const uint
            size = readInt(),
            len = size / sizeof(typename T::value_type);

        if (pos + size > msg_size)
            throw std::runtime_error{"Incomplete message"};

        T arr;
        arr.resize(len);

        std::memcpy(arr.data(), data + pos, size);

        pos += size;

        return arr;
    }

private:
    u8 *data;
    size_t msg_size{}, pos{};
};
