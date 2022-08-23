#pragma once

#include <jacky-common/defs.hpp>
#include <jacky-common/types.hpp>
#include <jcserver/player.hpp>
#include <variant>

enum ServerActionType {
    ADD_PLAYER,
    REMOVE_PLAYER,
    SEND_CHAT_MESSAGE,
};

struct AddPlayerData {
    ws::connection_hdl conn;
    std::wstring name;
};

struct RemovePlayerData {
    ws::connection_hdl conn;
};

struct SendChatMessageData {
    std::wstring message;
};

struct ServerAction {
    ServerActionType type;
    Player *player;
    std::variant<
        std::monostate,
        std::unique_ptr<AddPlayerData>,
        std::unique_ptr<RemovePlayerData>,
        std::unique_ptr<SendChatMessageData>
    > data;
};

