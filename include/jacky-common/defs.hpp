#pragma once

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <filesystem>

// INTEGERS

typedef std::int8_t i8;
typedef std::int16_t i16;
typedef std::int32_t i32;
typedef std::int64_t i64;

typedef unsigned int uint;

typedef std::uint8_t u8;
typedef std::uint16_t u16;
typedef std::uint32_t u32;
typedef std::uint64_t u64;

// WEBSOCKET

namespace ws = websocketpp;

typedef ws::client<ws::config::asio_client> WsClient;
typedef ws::server<ws::config::asio> WsServer;

namespace fs = std::filesystem;
