#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct ClientPlayer {
    std::wstring name;
    bool visible;

    glm::vec3 position, base_pos, target_pos;
    float yaw, base_yaw, target_yaw;
};

struct GameState {
    std::wstring name;
    std::vector<ClientPlayer> players;
};
