#include <jacky-common/message_reader.hpp>
#include <jacky-common/message_writer.hpp>
#include <jacky-common/client_actions.hpp>
#include <jcserver/server.hpp>
#include <utility>

Server::Server() : player_list(*this) {
    ws_server.init_asio();

    ws_server.set_open_handler(
        [this](auto &&conn) {
            onOpen(std::forward<decltype(conn)>(conn));
        });

    ws_server.set_close_handler(
        [this](auto &&conn) {
            onClose(std::forward<decltype(conn)>(conn));
        });

    ws_server.set_message_handler(
        [this](auto &&conn, auto &&msg) {
            onMessage(std::forward<decltype(conn)>(conn), std::forward<decltype(msg)>(msg));
        });
}

void Server::run(u16 port) {
    ws_server.listen(port);
    ws_server.start_accept();
    try {
        ws_server.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void Server::onOpen(ws::connection_hdl hdl) {
    {
        ws::lib::lock_guard<ws::lib::mutex> guard{actions_queue_lock};
        actions_queue.push(ServerAction{
            ADD_PLAYER, nullptr,
            std::make_unique<AddPlayerData>(AddPlayerData{move(hdl), L"zeubi"})
        });
        actions_queue.push(ServerAction{
            SEND_CHAT_MESSAGE, nullptr,
            std::make_unique<SendChatMessageData>(SendChatMessageData{L"zeubi joined"})
        });
    }
    actions_queue_cond.notify_one();
}

void Server::onClose(ws::connection_hdl hdl) {
    {
        ws::lib::lock_guard<ws::lib::mutex> guard{actions_queue_lock};
        actions_queue.push(ServerAction{
            REMOVE_PLAYER, nullptr,
            std::make_unique<RemovePlayerData>(RemovePlayerData{std::move(hdl)})
        });
    }
    actions_queue_cond.notify_one();
}

void Server::onMessage(const ws::connection_hdl &hdl, const WsServer::message_ptr &msg) {
    if (msg->get_payload().length() < 2)
        return;

    MessageReader reader{msg->get_payload()};
    const u16 message_type = reader.readUInt16();

    if (SEND_CHAT_MESSAGE == message_type) {
        if (msg->get_payload().length() <= 2)
            return;
        const auto message{reader.readArr<std::wstring>
            ((int) ((msg->get_payload().length() - 2) / sizeof(wchar_t)))};
        ws::lib::lock_guard<ws::lib::mutex> guard{actions_queue_lock};
        actions_queue.push(ServerAction{
            SEND_CHAT_MESSAGE, player_list.byConnection(hdl),
            std::make_unique<SendChatMessageData>(SendChatMessageData{message})
        });
    }
    actions_queue_cond.notify_one();
}

[[noreturn]] void Server::processMessages() {
    while (true) {
        ws::lib::unique_lock<ws::lib::mutex> lock{actions_queue_lock};

        while (actions_queue.empty())
            actions_queue_cond.wait(lock);

        ServerAction action = std::move(actions_queue.front());
        actions_queue.pop();

        lock.unlock();

        if (action.type == ADD_PLAYER) {
            auto &data = std::get<std::unique_ptr<AddPlayerData>>(action.data);
            ws::lib::lock_guard<ws::lib::mutex> guard{player_list_lock};
            player_list.addPlayer(data->name, data->conn);
        } else if (action.type == REMOVE_PLAYER) {
            auto &data = std::get<std::unique_ptr<RemovePlayerData>>(action.data);
            ws::lib::lock_guard<ws::lib::mutex> guard{player_list_lock};
            player_list.removePlayer(data->conn);
        } else if (action.type == SEND_CHAT_MESSAGE) {
            auto &data = std::get<std::unique_ptr<SendChatMessageData>>(action.data);
            ws::lib::lock_guard<ws::lib::mutex> guard{player_list_lock};

            MessageWriter msg;
            msg.writeInt16(ADD_CHAT_MESSAGE);
            if (action.player)
                msg.writeArr(L"<" + action.player->name + L"> " + data->message);
            else
                msg.writeArr(data->message);

            for (const auto &player: player_list)
                player.connection->send(msg.data(), msg.size());
        }
    }
}
