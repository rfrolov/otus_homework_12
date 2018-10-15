#pragma once

#include "boost/asio.hpp"
#include <functional>

namespace ba = boost::asio;

struct Server;

/// Класс ассинхронной работы с клиентом.
struct Client : std::enable_shared_from_this<Client> {
    Client(const Client &) = delete;

    void operator=(const Client &) = delete;

    explicit Client(Server &server, ba::io_service &service);

    /// Возвращает ссылку на сокет.
    ba::ip::tcp::socket &socket();


    /// Ничинает работу по обработке данных от клиента.
    void start();

    /// Заканчивает работу с клиентом.
    void stop();

private:

    void on_read(const boost::system::error_code &err, size_t data_size);

    ba::ip::tcp::socket m_socket;
    static const size_t m_read_buffer_size{10};
    char m_read_buffer[m_read_buffer_size];
    bool m_started;
    void *m_handle;
    Server &m_server;
};