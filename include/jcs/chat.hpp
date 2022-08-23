#pragma once

#include <deque>
#include <string>

class JSGame;

class Chat {
public:
    explicit Chat(JSGame &game);

    void initRender();

    void addMessage(const std::wstring& message);

    void render() const;

private:
    JSGame &game;
    std::deque<std::wstring> messages;

    uint vao, vbo;
    int triangle_count;

    uint max_width, chat_height;

    float animation_height, animation_start;
};
