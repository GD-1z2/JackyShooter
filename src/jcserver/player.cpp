#include <jacky-common/client_actions.hpp>
#include <jacky-common/message_writer.hpp>
#include <jcserver/player.hpp>
#include <jcserver/server.hpp>

void Player::allowJoin(glm::vec3 pos) {
    MessageWriter msg;
    msg.writeShort(ALLOW_JOIN);

    std::wstring name_list;
    for (const auto &p: server->player_list) {
        name_list += p.name;
        name_list += ';';
    }
    name_list.pop_back();
    msg.writeArr(name_list);

    msg.writeFloat(pos[0]);
    msg.writeFloat(pos[1]);
    msg.writeFloat(pos[2]);

    connection->send(msg.data(), msg.size());
}

void Player::sendChatMessage(const std::wstring &message) {
    MessageWriter msg;
    msg.writeShort(ADD_CHAT_MESSAGE);
    msg.writeArr(message);
    connection->send(msg.data(), msg.size());
}
