#include <jacky-common/client_actions.hpp>
#include <jacky-common/message_reader.hpp>
#include <jacky-common/message_writer.hpp>
#include <jcserver/server.hpp>
#include <random>
#include <utility>

static int randint(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

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
            onMessage(std::forward<decltype(conn)>(conn),
                      std::forward<decltype(msg)>(msg));
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
        actions_queue.push(AddPendingPlayerData::make(std::move(hdl)));
    }
    actions_queue_cond.notify_one();
}

void Server::onClose(ws::connection_hdl hdl) {
    {
        ws::lib::lock_guard<ws::lib::mutex> guard{actions_queue_lock};
        actions_queue.push(RemovePlayerData::make(std::move(hdl)));
    }
    actions_queue_cond.notify_one();
}

void Server::onMessage(const ws::connection_hdl &hdl,
                       const WsServer::message_ptr &msg) {
    if (msg->get_payload().length() < 2)
        return;

    MessageReader reader{msg->get_payload()};
    const u16 message_type = reader.readShort();

    if (ADD_PLAYER == message_type) {
        if (msg->get_payload().length() <= 2) // not enough space for msg
            return;

        const u16 jp_ver = reader.readShort();
        const auto name{reader.readArr<std::wstring>()};
        ws::lib::lock_guard<ws::lib::mutex> guard{actions_queue_lock};
        actions_queue.push(AddPlayerData::make(hdl, jp_ver, name));

    } else if (SEND_CHAT_MESSAGE == message_type) {
        if (msg->get_payload().length() <= 2) // not enough space for msg
            return;

        auto player_message = reader.readArr<std::wstring>();

        if (player_message.empty()
            || player_message.length() > SendChatMessageData::MAX_CHAT_LENGTH)
            return;

        if (player_message == L":players") {
            ws::lib::lock_guard<ws::lib::mutex> pc_guard{pending_conns_lock};
            std::wstring players =
                std::to_wstring(pending_conns.size()) + L" // " +
                std::to_wstring(player_list.size()) + L" : ";
            for (const auto &player: player_list) {
                players += player.name;
                players += L", ";
            }
            players.pop_back();
            players.pop_back();

            ws::lib::lock_guard<ws::lib::mutex> guard{actions_queue_lock};
            actions_queue.push(SendChatMessageData::make(nullptr, players));
        } else {
            ws::lib::lock_guard<ws::lib::mutex> guard{actions_queue_lock};
            actions_queue.push(SendChatMessageData::make(
                player_list.byConnection(hdl), std::move(player_message)));
        }
    }

    actions_queue_cond.notify_one();
}

[[noreturn]] void Server::processMessages() {
    while (true) {
        ws::lib::unique_lock<ws::lib::mutex> lock{actions_queue_lock};

        while (actions_queue.empty())
            actions_queue_cond.wait(lock);

        const ServerAction action = std::move(actions_queue.front());
        actions_queue.pop();

        lock.unlock();

        if (action.type == ADD_PENDING_PLAYER) {
            auto &data = action.get<AddPendingPlayerData>();
            ws::lib::lock_guard<ws::lib::mutex> guard{pending_conns_lock};
            pending_conns.push_back(data->hdl);

        } else if (action.type == ADD_PLAYER) {
            auto &data = action.get<AddPlayerData>();
            ws::lib::lock_guard<ws::lib::mutex> pl_guard{player_list_lock};

            if (data->jp_ver < JACKY_PROTOCOL_VERSION) {
                MessageWriter msg;
                msg.writeShort(REFUSE_JOIN);
                msg.writeArr<std::wstring>(
                    L"Votre jeu n'est pas compatible avec ce serveur car il"
                    "n'est pas à jour");
                ws_server.get_con_from_hdl(data->conn)
                    ->send(msg.data(), msg.size());
            } else if (data->jp_ver > JACKY_PROTOCOL_VERSION) {
                MessageWriter msg;
                msg.writeShort(REFUSE_JOIN);
                msg.writeArr<std::wstring>(
                    L"Votre jeu n'est pas compatible avec ce serveur car il est"
                    "trop récent");
                ws_server.get_con_from_hdl(data->conn)
                    ->send(msg.data(), msg.size());
            } else if (!AddPlayerData::validate(data->name)) {
                MessageWriter msg;
                msg.writeShort(REFUSE_JOIN);
                msg.writeArr<std::wstring>(
                    L"Votre nom ne doit pas contenir de caractères spéciaux"
                    "et doit contenir entre 1 et 24 caractères");
                ws_server.get_con_from_hdl(data->conn)
                    ->send(msg.data(), msg.size());
            } else if (!std::all_of(player_list.begin(), player_list.end(),
                                    [&](const Player &p) {
                                        return p.name != data->name;
                                    })) {
                MessageWriter msg;
                msg.writeShort(REFUSE_JOIN);
                msg.writeArr<std::wstring>(
                    L"Ce pseudo est déjà pris");
                ws_server.get_con_from_hdl(data->conn)
                    ->send(msg.data(), msg.size());
            } else {
                ws::lib::lock_guard<ws::lib::mutex> conns_guard{
                    pending_conns_lock};
                const auto it = std::find_if(
                    pending_conns.begin(), pending_conns.end(),
                    [&data](const ws::connection_hdl &hdl) {
                        return hdl.lock() == data->conn.lock();
                    });
                if (it != pending_conns.end()) {
                    pending_conns.erase(it);
                    Player &pl = player_list.addPlayer(data->name, data->conn);
                    pl.allowJoin(
                        {randint(-5, 5), 0, randint(-5, 5)});

                    MessageWriter msg;
                    msg.writeShort(PLAYER_LIST_ADD);
                    msg.writeArr(pl.name);

                    for (const auto &player: player_list)
                        if (player.name != pl.name)
                            player.connection->send(msg.data(), msg.size());
                } else {
                    std::cerr << "Pending connection not found / player tried"
                                 "to connect with name ";
                    std::wcerr << data->name << std::endl;
                }
            }


        } else if (action.type == REMOVE_PLAYER) {
            auto &data = action.get<RemovePlayerData>();
            try {
                ws::lib::lock_guard<ws::lib::mutex> guard{player_list_lock};

                const Player p = player_list.removePlayer(data->conn);
                MessageWriter msg;
                msg.writeShort(PLAYER_LIST_RMV);
                msg.writeArr(p.name);

                for (const auto &player: player_list)
                    player.connection->send(msg.data(), msg.size());

            } catch (std::exception &) {
                ws::lib::lock_guard<ws::lib::mutex> guard{pending_conns_lock};

                const auto it = std::find_if(
                    pending_conns.begin(), pending_conns.end(),
                    [&data](const ws::connection_hdl &hdl) {
                        return hdl.lock() == data->conn.lock();
                    });
                if (it != pending_conns.end())
                    pending_conns.erase(it);
            }

        } else if (action.type == SEND_CHAT_MESSAGE) {
            auto &data = action.get<SendChatMessageData>();
            ws::lib::lock_guard<ws::lib::mutex> guard{player_list_lock};

            MessageWriter msg;
            msg.writeShort(ADD_CHAT_MESSAGE);
            if (action.player)
                msg.writeArr(
                    L"<" + action.player->name + L"> " + data->message);
            else
                msg.writeArr(data->message);

            for (const auto &player: player_list)
                player.connection->send(msg.data(), msg.size());
        }

        next_action:;
    }
}
