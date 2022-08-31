#pragma once

#include <jacky-common/client_actions.hpp>
#include <jacky-common/defs.hpp>

class Connection {
public:
    explicit Connection(const std::string &uri);

    void disconnect();

    // Protocol

    void join(const std::wstring &name);

    void sendChatMessage(const std::wstring &message);

    void syncPos(const glm::vec3 &pos, float yaw);

    WsClient ws_client;
    WsClient::connection_ptr ws_connection;
    std::thread ws_thread;
    std::queue<ClientAction> actions;
    ws::lib::mutex actions_lock;
};
