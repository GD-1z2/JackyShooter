#pragma once

#include <deque>
#include <jcs/screen.hpp>
#include <string>

class GameScreen;

class Chat {
public:
    typedef std::deque<std::wstring> MessageList;

    explicit Chat(JSGame &game);

    void initRender();

    void addMessage(const std::wstring &message);

    void updateBuffer();

    void render() const;

    MessageList messages;
    bool chat_screen_opened{false};
private:
    JSGame &game;

    uint vao{}, vbo{};
    int triangle_count{};

    uint chat_height{};

    float animation_height{}, animation_start{};
};

class ChatScreen : public Screen {
public:
    /**
     * @brief Chat screen overlay
     * @note Must be created with a parent screen
     */
    ChatScreen(JSGame &game, Chat &chat);

    ~ChatScreen() override;

    void update() override;

    void render() override;

    void onResize() override;

    void onFocus() override;

    bool onKey(int key, int scancode, int action, int mods) override;

    void onChar(uint codepoint) override;

private:
    GameScreen &parent_screen;
    Chat &chat;
    GuiTextInput textInput;
    bool can_type{false};
};
