#pragma once

#include <iostream>
#include <jcserver/types.hpp>

class MessageWriter {
public:
    template<typename T>
    void writeArr(const T &arr) {
        const int end = (int) vec.size();
        vec.resize(end + arr.size() * sizeof(typename T::value_type));
        std::copy(arr.data(), arr.data() + arr.size() * sizeof(typename T::value_type), vec.begin() + end);
    }

    void writeInt16(i16 val) {
        const int end = (int) vec.size();
        vec.resize(end + 2);

        vec[end] = (u8) ((val >> 8) & 0xFF);
        vec[end + 1] = (u8) (val & 0xFF);
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
