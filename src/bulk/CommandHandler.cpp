#include "CommandHandler.h"
#include <cassert>
#include "print.h"

CommandHandler::CommandHandler(std::size_t block_size) : m_block_size{block_size} {
    assert(block_size != 0);
}

void CommandHandler::set_block_size(size_t block_size) {
    m_block_size = block_size;
}

size_t CommandHandler::add_printer(const fn_printer_t &printer) {
    m_printers.emplace_back(printer);
    return m_printers.size();
}

void CommandHandler::del_printer(size_t printer_id) {
    if (printer_id && printer_id - 1 < m_printers.size()) {
        m_printers.erase(std::next(m_printers.begin(), printer_id - 1));
    }
}

void CommandHandler::add_command(const std::string &command) {
    ++m_statistic.string_num;
    if (command == "{") {
        print_pool();
        ++m_braces_num;
    } else if (command == "}") {
        if (m_braces_num) {
            --m_braces_num;
            print_pool();
        }
    } else {
        if (m_command_pool.empty()) {
            m_first_command_time = std::time(nullptr);
        }
        m_command_pool.emplace_back(command);
        if (m_command_pool.size() == m_block_size) {
            print_pool();
        }
    }
}

CommandHandler::Statistic CommandHandler::finish() {
    print_pool();
    Statistic result = m_statistic;
    m_statistic = Statistic{};
    return result;
}

CommandHandler::~CommandHandler() {
    finish();
}

void CommandHandler::print_pool() {
    if (m_command_pool.empty() || m_braces_num) {
        return;
    }

    for (const auto &printer:m_printers) {
        printer(m_first_command_time, m_command_pool);
    }

    ++m_statistic.bulk_num;
    m_statistic.command_num += m_command_pool.size();
    m_command_pool.clear();
}


