#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct ClientPlayer {
    std::wstring name;
    bool visible;
    glm::vec3 position;
};

struct GameState {
    std::vector<ClientPlayer> players;
};
