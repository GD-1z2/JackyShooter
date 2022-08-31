#include <jcserver/player_list.hpp>
#include <jcserver/server.hpp>
#include <utility>

PlayerList::PlayerList(Server &server) : server(server) {}

Player &
PlayerList::addPlayer(std::wstring name, ws::connection_hdl connection) {
    player_vec.push_back(
        {
            &server,
            std::move(name),
            server.ws_server.get_con_from_hdl(
                std::move(connection)),
        });

    return player_vec.back();
}

Player PlayerList::removePlayer(const ws::connection_hdl &connection) {
    const auto it = std::find_if(
        player_vec.begin(), player_vec.end(),
        [&](const Player &p) {
            return server.ws_server.get_con_from_hdl(p.connection)
                   == server.ws_server.get_con_from_hdl(connection);
        });

    if (it == player_vec.end())
        throw std::runtime_error{"Trying to remove unknown it"};

    Player p = *it;
    player_vec.erase(it);
    return p;
}

Player *PlayerList::byConnection(const ws::connection_hdl &connection) {
    const auto player = std::find_if(
        player_vec.begin(), player_vec.end(),
        [&](const Player &p) {
            return server.ws_server.get_con_from_hdl(p.connection)
                   == server.ws_server.get_con_from_hdl(connection);
        });

    return player != player_vec.end() ? &*player : nullptr;
}

size_t PlayerList::size() const {
    return player_vec.size();
}
