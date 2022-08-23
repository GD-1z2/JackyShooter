#include <jcserver/player_list.hpp>
#include <jcserver/server.hpp>
#include <utility>

PlayerList::PlayerList(Server &server) : server(server) {}

void PlayerList::addPlayer(std::wstring name, ws::connection_hdl connection) {
    player_vec.push_back(
        {
            .name = std::move(name),
            .connection = server.ws_server.get_con_from_hdl(std::move(connection)),
        });
}

void PlayerList::removePlayer(const ws::connection_hdl &connection) {
    const auto player = std::find_if(player_vec.begin(), player_vec.end(), [&](const Player &p) {
        return server.ws_server.get_con_from_hdl(p.connection) == server.ws_server.get_con_from_hdl(connection);
    });

    if (player == player_vec.end())
        throw std::runtime_error{"Trying to remove unknown player"};

    player_vec.erase(player);
}

Player *PlayerList::byConnection(const ws::connection_hdl &connection) {
    const auto player = std::find_if(player_vec.begin(), player_vec.end(), [&](const Player &p) {
        return server.ws_server.get_con_from_hdl(p.connection) == server.ws_server.get_con_from_hdl(connection);
    });

    return player != player_vec.end() ? &*player : nullptr;
}
