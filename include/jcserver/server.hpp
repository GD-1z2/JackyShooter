#pragma once

#include <iostream>
#include <jacky-common/defs.hpp>
#include <jacky-common/server_actions.hpp>
#include <jcserver/player_list.hpp>
#include <jcserver/player.hpp>
#include <jcserver/types.hpp>
#include <set>
#include <websocketpp/common/thread.hpp>

class Server {
public:
    Server();

    void run(u16 port);

    void onOpen(ws::connection_hdl hdl);

    void onClose(ws::connection_hdl hdl);

    void onMessage(const ws::connection_hdl& hdl, const WsServer::message_ptr& msg);

    [[noreturn]] void processMessages();

    WsServer ws_server;
    PlayerList player_list;
private:
    ws::lib::mutex player_list_lock;

    std::queue<ServerAction> actions_queue;
    ws::lib::mutex actions_queue_lock;
    ws::lib::condition_variable actions_queue_cond;

    std::vector<ws::connection_hdl> pending_conns;
    ws::lib::mutex pending_conns_lock;
};
