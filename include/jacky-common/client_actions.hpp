#pragma once

#include <memory>
#include <string>
#include <variant>

enum ClientActionType {
    ADD_CHAT_MESSAGE
};

struct AddChatMessageData {
    std::string message;
};

struct ClientAction {
    ClientActionType type;
    std::variant<std::monostate, std::unique_ptr<AddChatMessageData>> data;
};
