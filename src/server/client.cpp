#include "client.h"
#include "server.h"
#include "bulk/CommandProcessor.h"

ba::ip::tcp::socket &Client::socket() { return m_socket; }

Client::Client(Server &server, ba::io_service &service) : m_server{server}
                                                          , m_socket{service}
                                                          , m_started{false} {
}

void Client::start() {
    m_handle = CmdProcessor::getInstance().create();
    m_started = true;
    m_socket.async_read_some(ba::buffer(m_read_buffer), [this](const boost::system::error_code &err, size_t bytes) {
        on_read(err, bytes);
    });
}

void Client::stop() {
    if (!m_started) return;
    CmdProcessor::getInstance().destroy(m_handle);
    m_started = false;
    m_socket.close();
    m_server.remove_client(shared_from_this());
}

void Client::on_read(const boost::system::error_code &err, size_t data_size) {
    if (!m_started) {
        return;
    }

    if (err) {
        stop();
        return;
    }

    CmdProcessor::getInstance().process(m_handle, std::string(&m_read_buffer[0], &m_read_buffer[data_size]));
    m_socket.async_read_some(ba::buffer(m_read_buffer), [this](const boost::system::error_code &err, size_t bytes) {
        on_read(err, bytes);
    });
}
