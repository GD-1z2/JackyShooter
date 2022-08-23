#include <jcs/game_screen.hpp>
#include <jacky-common/message_reader.hpp>

bool GameScreen::connect(const std::string &uri) {
    try {
        ws_client.set_access_channels(ws::log::alevel::all);
        ws_client.clear_access_channels(ws::log::alevel::frame_header);
        ws_client.set_error_channels(ws::log::elevel::all);

        ws_client.init_asio();

        ws_client.set_message_handler(
            [this](const ws::connection_hdl &,
                   const ws::connection<ws::config::asio_client>::message_ptr &msg) {
                const auto &msg_s = msg->get_payload();
                std::cout << msg_s << std::endl;
                if (msg_s.length() < 2)
                    return;

                MessageReader reader{msg_s};

                const u16 type = reader.readUInt16();

                if (ADD_CHAT_MESSAGE == type) {
                    const auto message = reader.readArr<std::string>((int) msg_s.size() - 2);

                    ws::lib::lock_guard<ws::lib::mutex> guard{actions_queue_lock};
                    actions_queue.push(ClientAction{
                        ADD_CHAT_MESSAGE,
                        std::make_unique<AddChatMessageData>(AddChatMessageData{message})
                    });
                }
            });

        ws::lib::error_code ec;
        ws_connection = ws_client.get_connection(uri, ec);

        if (ec) {
            std::cerr << "Connection failed : " << ec.message() << std::endl;
            return false;
        }

        ws_client.connect(ws_connection);

        ws_thread = std::thread{[this] { ws_client.run(); }};

        return true;
    } catch (ws::exception const &e) {
        std::cerr << "Exception occurred during connection:  " << e.what() << std::endl;
    }

    return false;
}
