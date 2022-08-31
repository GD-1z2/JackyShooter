#pragma once

#include <jacky-common/defs.hpp>
#include <jcserver/player.hpp>
#include <utility>
#include <variant>

/**
 * Very important constant used to verify if players who try to connect run the
 * same version of the game as the server.
 */
static const u16 JACKY_PROTOCOL_VERSION = 0;

enum ServerActionType {
    /**
     * Action created when a player tries to connect.
     */
    ADD_PENDING_PLAYER,

    /**
     * Action sent by a player who wants to pick a name and join the game.
     */
    ADD_PLAYER,

    /**
     * Action sent by a (possibly pending)  player who wants to disconnect.
     */
    REMOVE_PLAYER,

    /**
     * Action sent by a player or created by the server to send a message to the
     * chat.
     */
    SEND_CHAT_MESSAGE,
};

struct AddPendingPlayerData;
struct AddPlayerData;
struct RemovePlayerData;
struct SendChatMessageData;

struct ServerAction {
    /**
     * The type of the action.
     */
    ServerActionType type;

    /**
     * If the action was created because of a received packet, a valid pointer
     * to the data of the player who sent it. nullptr otherwise.
     */
    Player *player;

    /**
     * The data of the action.
     */
    std::variant<
        std::monostate,

        std::unique_ptr<AddPendingPlayerData>,
        std::unique_ptr<AddPlayerData>,
        std::unique_ptr<RemovePlayerData>,
        std::unique_ptr<SendChatMessageData>
    > data;

    template<typename T>
    const std::unique_ptr<T> &get() const {
        return std::get<std::unique_ptr<T>>(data);
    }
};

struct AddPendingPlayerData {
    ws::connection_hdl hdl;

    static ServerAction make(ws::connection_hdl hdl) {
        return ServerAction{
            ADD_PENDING_PLAYER, nullptr,
            std::make_unique<AddPendingPlayerData>(AddPendingPlayerData{
                std::move(hdl)
            })
        };
    }
};

struct AddPlayerData {
    ws::connection_hdl conn;
    u16 jp_ver;
    std::wstring name;

    static ServerAction make(ws::connection_hdl conn, u16 jp_ver,
                             std::wstring name) {
        return ServerAction{
            ADD_PLAYER, nullptr,
            std::make_unique<AddPlayerData>(AddPlayerData{
                std::move(conn), jp_ver, std::move(name)
            })
        };
    }

    static bool validate(const std::wstring &name) {
        return !name.empty() && name.size() <= 24 &&
               std::all_of(name.begin(), name.end(), [](wchar_t c) {
                   return iswalpha(c) || iswdigit(c) || c == L'_';
               });
    }
};

struct RemovePlayerData {
    ws::connection_hdl conn;

    static ServerAction make(ws::connection_hdl conn) {
        return ServerAction{
            REMOVE_PLAYER, nullptr,
            std::make_unique<RemovePlayerData>(RemovePlayerData{
                std::move(conn)
            })
        };
    }
};

struct SendChatMessageData {
    std::wstring message;

    static const u16 MAX_CHAT_LENGTH = 256;

    static ServerAction make(Player *player, std::wstring message) {
        return ServerAction{
            SEND_CHAT_MESSAGE, player,
            std::make_unique<SendChatMessageData>(SendChatMessageData{
                std::move(message)
            })
        };
    }
};
