#pragma once

#include "boost/asio.hpp"
#include <functional>
#include <list>


namespace ba = boost::asio;

struct Client;

/// Класс сервера.
struct Server {
    Server(const Server &) = delete;

    void operator=(const Server &) = delete;

    /**
     * Конструктор.
     * @param port Номер порта, по которому будут приниматься данные.
     * @param block_size Размер блока команд.
     */
    explicit Server(uint16_t port, size_t block_size);

    /// Запускает сервер.
    void run();

private:
    void do_accept();
    void do_stop();

    uint16_t              m_port;
    ba::io_service        m_service;
    ba::ip::tcp::acceptor m_acceptor;
    ba::ip::tcp::socket   m_socket;
};