#pragma once

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <filesystem>

namespace ws = websocketpp;

typedef ws::client<ws::config::asio_client> WsClient;
typedef ws::server<ws::config::asio> WsServer;

namespace fs = std::filesystem;
