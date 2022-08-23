#pragma once

#include <jcserver/player.hpp>
#include <vector>

class Server;

class PlayerList {
public:
    PlayerList(Server &server);

    void addPlayer(std::wstring name, ws::connection_hdl connection);

    void removePlayer(const ws::connection_hdl &connection);

    Player *byConnection(const ws::connection_hdl& connection);

    // Iterator stuff

    inline std::vector<Player>::iterator begin() noexcept { return player_vec.begin(); }
    inline std::vector<Player>::const_iterator cbegin() const noexcept { return player_vec.cbegin(); }
    inline std::vector<Player>::iterator end() noexcept { return player_vec.end(); }
    inline std::vector<Player>::const_iterator cend() const noexcept { return player_vec.cend(); }

private:
    Server &server;
    std::vector<Player> player_vec;
};

