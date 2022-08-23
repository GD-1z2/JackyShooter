#pragma once

#include <jacky-common/defs.hpp>
#include <jacky-common/types.hpp>
#include <websocketpp/server.hpp>

struct Player {
    void sendChatMessage(const std::wstring &message);

    std::wstring name;
    WsServer::connection_ptr connection;
    float x, y, z;
};
