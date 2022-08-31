#pragma once

#include <glm/glm.hpp>
#include <jacky-common/defs.hpp>

class Server;

struct Player {
    void allowJoin(glm::vec3 pos);

    void sendChatMessage(const std::wstring &message);

    Server *server;
    std::wstring name;
    WsServer::connection_ptr connection;
    float x, y, z;
};
