#include "server.h"
#include "client.h"
#include "bulk/CommandProcessor.h"

Server::Server(uint16_t port, size_t block_size)
        : m_port{port}, m_block_size{block_size},
          m_acceptor{m_service, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), m_port)},
          m_handle{CmdProcessor::getInstance().create(block_size)} {
}

Server::~Server() {
    CmdProcessor::getInstance().destroy(m_handle);
}

void Server::run() {
    clients.emplace_back(std::make_shared<Client>(*this, m_service, m_handle));
    auto client = clients.back();
    m_acceptor.async_accept(client->socket(),
                            [this, &client](const boost::system::error_code &err) { handle_accept(client, err); });
    m_service.run();
}

void Server::remove_client(const client_t &client) {
    auto it = std::find(clients.begin(), clients.end(), client);
    clients.erase(it);
}

void Server::handle_accept(const client_t &client, const boost::system::error_code &err) {
    if (!err) {
        client->start(m_block_size);
    }

    clients.emplace_back(std::make_shared<Client>(*this, m_service, m_handle));
    auto &new_client = clients.back();
    m_acceptor.async_accept(new_client->socket(), [this, &new_client](const boost::system::error_code &err) {
        handle_accept(new_client, err);
    });
}
