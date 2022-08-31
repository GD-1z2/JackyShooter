#include <jacky-common/message_reader.hpp>
#include <jacky-common/message_writer.hpp>
#include <jacky-common/server_actions.hpp>
#include <jcs/connection.hpp>

Connection::Connection(const std::string &uri) {
    try {
        ws_client.set_access_channels(ws::log::alevel::all);
        ws_client.clear_access_channels(ws::log::alevel::frame_header);
        ws_client.set_error_channels(ws::log::elevel::all);

        ws_client.init_asio();

        ws_client.set_open_handler(
            [&](const ws::connection_hdl &hdl) {
                std::cout << "Connection opened" << std::endl;

                ws::lib::lock_guard<ws::lib::mutex> guard{actions_lock};
                actions.push(ClientAction{CONNECT});
            }
        );

        ws_client.set_close_handler(
            [](const ws::connection_hdl &hdl) {
                std::cout << "Disconnected" << std::endl;
            }
        );

        ws_client.set_message_handler([this](
            const ws::connection_hdl &,
            const ws::connection<ws::config::asio_client>::message_ptr &msg) {

            const auto &msg_s = msg->get_payload();
            std::cout << msg_s << std::endl;

            if (msg_s.length() < 2) // Not enough space for packet ID
                return;

            MessageReader reader{msg_s};

            const u16 type = reader.readShort();

            if (REFUSE_JOIN == type) {
                ws::lib::lock_guard<ws::lib::mutex> guard{actions_lock};
                actions.push(RefuseJoinData::make(
                    reader.readArr<std::wstring>()));

            } else if (ALLOW_JOIN == type) {
                const auto name_str = reader.readArr<std::wstring>();
                std::vector<std::wstring> name_list;
                wchar_t delim = ';';

                uint start{0};
                auto end{name_str.find(delim)};
                while (end != std::string::npos) {
                    name_list.push_back(name_str.substr(start, end - start));
                    start = end + 1;
                    end = name_str.find(delim, start);
                }
                name_list.push_back(name_str.substr(start, end));

                glm::vec3 pos{reader.readFloat(), reader.readFloat(),
                              reader.readFloat()};

                ws::lib::lock_guard<ws::lib::mutex> guard{actions_lock};
                actions.push(AllowJoinData::make(name_list, pos));

            } else if (ADD_CHAT_MESSAGE == type) {
                const auto message = reader.readArr<std::wstring>();
                ws::lib::lock_guard<ws::lib::mutex> guard{actions_lock};
                actions.push(AddChatMessageData::make(message));

            } else if (PLAYER_LIST_ADD == type) {
                const auto name = reader.readArr<std::wstring>();
                ws::lib::lock_guard<ws::lib::mutex> guard{actions_lock};
                actions.push(PlayerListAddData::make(name));

            } else if (PLAYER_LIST_RMV == type) {
                const auto name = reader.readArr<std::wstring>();
                ws::lib::lock_guard<ws::lib::mutex> guard{actions_lock};
                actions.push(PlayerListRmvData::make(name));
            }
        });

        ws::lib::error_code ec;
        ws_connection = ws_client.get_connection(uri, ec);
        if (ec)
            throw std::runtime_error{"Connection failed : " + ec.message()};

        ws_client.connect(ws_connection);

        ws_thread = std::thread{[this] { ws_client.run(); }};
    } catch (ws::exception const &e) {
        std::cerr << "Exception occurred during connection:  " << e.what()
                  << std::endl;
    }
}

void Connection::disconnect() {
    if (ws_connection->get_state() == ws::session::state::open)
        ws_connection->close(ws::close::status::normal, "Disconnect");
    ws_thread.join();
}

void Connection::join(const std::wstring &name) {
    MessageWriter msg{};
    msg.writeShort(ADD_PLAYER);
    msg.writeShort(JACKY_PROTOCOL_VERSION);
    msg.writeArr(name);
    ws_connection->send(msg.data(), msg.size(), ws::frame::opcode::binary);
}

void Connection::sendChatMessage(const std::wstring &message) {
    MessageWriter msg{};
    msg.writeShort(ADD_CHAT_MESSAGE);
    msg.writeArr(message);
    ws_connection->send(msg.data(), msg.size(), ws::frame::opcode::binary);
}
