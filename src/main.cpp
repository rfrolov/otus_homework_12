#include "server/server.h"
#include <iostream>

#include <csignal>

std::unique_ptr<Server> app{nullptr};

void signal_handler(int) {
    app->stop();
}

int main(int argc, char *argv[]) {
    uint16_t port{0};
    size_t   block_size{0};

    if (argc == 3) {
        port       = static_cast<std::uint16_t>(strtoll(argv[1], nullptr, 0));
        block_size = static_cast<std::size_t>(strtoll(argv[2], nullptr, 0));
    }

    if (port == 0 || block_size == 0) {
        std::cout << "Wrong argument." << std::endl;
        return 1;
    }

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    app = std::make_unique<Server>(port, block_size);
    app->run();
    return 0;
}
