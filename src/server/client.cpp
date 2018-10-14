#include "client.h"
#include "server.h"

ba::ip::tcp::socket &Client::socket() { return m_socket; }

void Client::start(size_t block_size) {
    m_started = true;
    m_handle  = async::connect(block_size);

    m_socket.async_read_some(ba::buffer(m_read_buffer), [this](const boost::system::error_code &err, size_t bytes) {
        on_read(err, bytes);
    });
}

void Client::stop() {
    if (!m_started) return;
    m_started = false;
    m_socket.close();
    async::disconnect(m_handle);

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

    async::receive(m_handle, m_read_buffer, data_size);

    m_socket.async_read_some(ba::buffer(m_read_buffer), [this](const boost::system::error_code &err, size_t bytes) {
        on_read(err, bytes);
    });
}
