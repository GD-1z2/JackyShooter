#include <jcserver/server.hpp>

int main() {
    try {
        Server server_instance;

        ws::lib::thread thr{[&] {
            server_instance.processMessages();
        }};

        server_instance.run(9002);

        thr.join();
    } catch (websocketpp::exception const &e) {
        std::cout << e.what() << std::endl;
    }
}
