#include "server.h"
#include "client.h"

void Server::run() {
    clients.emplace_back(std::make_shared<Client>(*this, m_service));
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

    clients.emplace_back(std::make_shared<Client>(*this, m_service));
    auto &new_client = clients.back();
    m_acceptor.async_accept(new_client->socket(), [this, &new_client](const boost::system::error_code &err) {
        handle_accept(new_client, err);
    });
}
