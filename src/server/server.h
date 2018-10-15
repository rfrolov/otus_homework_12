#pragma once

#include "boost/asio.hpp"
#include <functional>
#include <list>


namespace ba = boost::asio;

struct Client;

/// Класс сервера.
struct Server {
    using client_t = std::shared_ptr<Client>;

    Server(const Server &) = delete;

    void operator=(const Server &) = delete;

    /**
     * Конструктор.
     * @param port Номер порта, по которому будут приниматься данные.
     * @param block_size Размер блока команд.
     */
    explicit Server(uint16_t port, size_t block_size);

    ~Server();

    /// Запускает сервер.
    void run();

    /**
     * Удаляет клиента.
     * @param client Адрес клиента.
     */
    void remove_client(const client_t &client);

private:
    void handle_accept(const client_t &client, const boost::system::error_code &err);

    uint16_t              m_port;
    size_t                m_block_size;
    ba::io_service        m_service;
    ba::ip::tcp::acceptor m_acceptor;
    std::list<client_t>   clients{};
    void *                m_handle;
};