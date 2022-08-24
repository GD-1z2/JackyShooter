#pragma once

#include <cstring>
#include <iostream>
#include <jacky-common/types.hpp>
#include <vector>
#include <netinet/in.h>

class MessageWriter {
public:
    void writeByte(u8 val) {
        vec.push_back(val);
    }

    void writeShort(u16 val) {
        vec.push_back((val >> 8) & 0xFF);
        vec.push_back(val & 0xFF);
    }

    void writeInt(u32 val) {
        vec.push_back((val >> 24) & 0xFF);
        vec.push_back((val >> 16) & 0xFF);
        vec.push_back((val >> 8) & 0xFF);
        vec.push_back(val & 0xFF);
    }

    void writeString(const std::string &str) {
        writeInt(str.size());
        vec.insert(vec.end(), str.begin(), str.end());
    }

    template<typename T>
    void writeArr(const T &arr) {
        const int size = (arr.size() * sizeof(typename T::value_type));
        writeInt(size);
        vec.resize(vec.size() + size);
        std::memcpy(vec.data() + vec.size() - size, arr.data(), size);
    }

    void *data() {
        return vec.data();
    }

    size_t size() {
        return vec.size();
    }

private:
    std::vector<u8> vec;
};
