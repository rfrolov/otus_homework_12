#include "server.h"
#include "client.h"
#include "bulk/CommandProcessor.h"

Server::Server(uint16_t port, size_t block_size) : m_port{port}
                                                   , m_acceptor{m_service,
                                                                ba::ip::tcp::endpoint(ba::ip::tcp::v4(), m_port)} {
    CmdProcessor::getInstance().set_block_size(block_size);
}

void Server::run() {
    clients.emplace_back(std::make_shared<Client>(*this, m_service));
    auto client = clients.back();
    m_acceptor.async_accept(client->socket(),
                            [this, &client](const boost::system::error_code &err) { handle_accept(client, err); });
    m_service.run();
}

void Server::stop() {
    m_service.stop();
    for(const auto& it : clients) {
        it->stop();
    }
}

void Server::remove_client(const client_t &client) {
    auto it = std::find(clients.begin(), clients.end(), client);
    clients.erase(it);
}

void Server::handle_accept(const client_t &client, const boost::system::error_code &err) {
    if (!err) {
        client->start();
    }

    clients.emplace_back(std::make_shared<Client>(*this, m_service));
    auto &new_client = clients.back();
    m_acceptor.async_accept(new_client->socket(), [this, &new_client](const boost::system::error_code &err) {
        handle_accept(new_client, err);
    });
}
