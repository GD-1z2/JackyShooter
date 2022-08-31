#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

enum ClientActionType {
    /**
     * Action created by the client after a successful connection to a server.
     */
    CONNECT,

    /**
     * Action sent by the server to refuse a player's connection.
     */
    REFUSE_JOIN,

    /**
     * Action sent by the server to allow the client to join the game.
     */
    ALLOW_JOIN,

    /**
     * Action sent by the server to push a message to the chat.
     */
    ADD_CHAT_MESSAGE,

    /**
     * Action sent by the server when a player joins.
     */
    PLAYER_LIST_ADD,

    /**
     * Action sent by the server when a player quits.
     */
    PLAYER_LIST_RMV,

    /**
     * Action sent by the server to update all players positions.
     */
    PLAYER_LIST_SET_POS,
};

struct RefuseJoinData;
struct AllowJoinData;
struct AddChatMessageData;
struct PlayerListAddData;
struct PlayerListRmvData;
struct PlayerListSetPosData;

struct ClientAction {
    /**
     * Type of the action.
     */
    ClientActionType type;

    /**
     * The data of the action.
     */
    std::variant<
        std::monostate,
        std::unique_ptr<RefuseJoinData>,
        std::unique_ptr<AllowJoinData>,
        std::unique_ptr<AddChatMessageData>,
        std::unique_ptr<PlayerListAddData>,
        std::unique_ptr<PlayerListRmvData>,
        std::unique_ptr<PlayerListSetPosData>
    > data;

    template<typename T>
    const std::unique_ptr<T> &get() const {
        return std::get<std::unique_ptr<T>>(data);
    }
};

struct RefuseJoinData {
    std::wstring message;

    static ClientAction make(const std::wstring &message) {
        return ClientAction{
            REFUSE_JOIN,
            std::make_unique<RefuseJoinData>(RefuseJoinData{
                message
            })
        };
    }
};

struct AllowJoinData {
    std::vector<std::wstring> online_players;
    glm::vec3 spawn_pos;

    static ClientAction make(std::vector<std::wstring> online_players,
                             glm::vec3 spawn_pos) {
        return ClientAction{
            ALLOW_JOIN,
            std::make_unique<AllowJoinData>(AllowJoinData{
                std::move(online_players), spawn_pos
            })
        };
    }
};

struct AddChatMessageData {
    std::wstring message;

    static ClientAction make(const std::wstring &message) {
        return ClientAction{
            ADD_CHAT_MESSAGE,
            std::make_unique<AddChatMessageData>(AddChatMessageData{
                message
            })
        };
    }
};

struct PlayerListAddData {
    std::wstring name;

    static ClientAction make(const std::wstring &name) {
        return ClientAction{
            PLAYER_LIST_ADD,
            std::make_unique<PlayerListAddData>(PlayerListAddData{
                name
            })
        };
    }
};

struct PlayerListRmvData {
    std::wstring name;

    static ClientAction make(const std::wstring &name) {
        return ClientAction{
            PLAYER_LIST_RMV,
            std::make_unique<PlayerListRmvData>(PlayerListRmvData{
                name
            })
        };
    }
};

struct PlayerListSetPosData {
    struct PlData {
        std::wstring name;
        glm::vec3 pos;
        float yaw;
    };

    std::vector<PlData> player_positions;

    static ClientAction make(const std::vector<PlData> &player_positions) {
        return ClientAction{
            PLAYER_LIST_SET_POS,
            std::make_unique<PlayerListSetPosData>(PlayerListSetPosData{
                player_positions
            })
        };
    }
};
