#include <jacky-common/message_writer.hpp>
#include <jcserver/player.hpp>
#include <utility>

void Player::sendChatMessage(const std::wstring &message) {
    MessageWriter msg;
    msg.writeShort(0);
    msg.writeArr(message);
    connection->send(msg.data(), msg.size());
}
