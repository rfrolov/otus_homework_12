#include "server.h"
#include "client.h"
#include "bulk/CommandProcessor.h"

Server::Server(uint16_t port, size_t block_size) :
        m_port{port}
        , m_acceptor{m_service, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), m_port)}
        , m_socket{m_service} {
    CmdProcessor::getInstance().set_block_size(block_size);
}

void Server::run() {
    ba::signal_set signals(m_service, SIGINT, SIGTERM);
    signals.async_wait(
            [this, &signals](const boost::system::error_code &/*error*/, int /*signal_number*/) { do_stop(); });
    do_accept();
    m_service.run();
}

void Server::do_stop() {
    m_service.stop();
}

void Server::do_accept() {
    m_acceptor.async_accept(m_socket, [this](const boost::system::error_code &err) {
        if (!err) {
            std::make_shared<Client>(std::move(m_socket))->start();
        }
        do_accept();
    });
}
