#include "CommandProcessor.h"

#include <iostream>
#include <algorithm>
#include "print.h"

std::ostream &operator<<(std::ostream &os, const CommandHandler::Statistic &statistic) {
    return os << "строк: " << statistic.string_num << ", блоков: " << statistic.bulk_num << ", команд: "
              << statistic.command_num;
}

std::ostream &operator<<(std::ostream &os, const ThreadPool::Statistic &statistic) {
    return os << "блоков: " << statistic.bulk_num << ", команд: " << statistic.command_num;
}

void CmdProcessor::set_block_size(std::size_t block_size) {
    m_command_handler.set_block_size(block_size);
}

CmdProcessor::context_t CmdProcessor::create() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_handles.emplace_back(std::make_unique<Context>());

    if (m_print_log_id == 0) {
        m_print_log_id = m_command_handler.add_printer(
                [this](std::time_t, CommandHandler::command_pull_t command_pull) {
                    m_thread_pool.enqueue([command_pull] { return print_log(command_pull); });
                });
    }

    if (m_print_file_id == 0) {
        m_print_file_id = m_command_handler.add_printer(
                [this](std::time_t time, CommandHandler::command_pull_t command_pull) {
                    static size_t rnd{};
                    m_thread_pool.enqueue([time, command_pull] {
                        return print_file(time, rnd++, command_pull);
                    });
                });
    }
    return m_handles.back().get();
}

void CmdProcessor::process(void *handle, const std::string &new_data) {
    std::unique_lock<std::mutex> lock(m_mutex);

    auto it = std::find_if(m_handles.cbegin(), m_handles.cend(), [&handle](auto &it) { return it.get() == handle; });
    if (it == m_handles.cend()) { return; }
    auto &context = *it;

    std::string data      = context->remaining_data + new_data;
    std::size_t data_size = data.length();

    std::size_t      last_index{};
    for (std::size_t index{0}; index < data_size; ++index) {
        if (data[index] == '\n') {
            m_command_handler.add_command(std::string(&data[last_index], &data[index]));
            last_index = index + 1;
        }
    }
    context->remaining_data = last_index < data_size ? std::string(&data[last_index], &data[data_size]) : "";
}

void CmdProcessor::destroy(void *handle) {
    std::unique_lock<std::mutex> lock(m_mutex);

    auto it = std::find_if(m_handles.cbegin(), m_handles.cend(), [&handle](auto &it) { return it.get() == handle; });
    if (it == m_handles.cend()) { return; }
    auto &context = *it;

    if (m_handles.empty()) {
        m_command_handler.del_printer(m_print_log_id);
        m_print_log_id = 0;

        m_command_handler.del_printer(m_print_file_id);
        m_print_file_id = 0;
    }

    m_handles.erase(it);
}

CmdProcessor::~CmdProcessor() {
    auto cmd_statistic  = m_command_handler.finish();
    auto pool_statistic = m_thread_pool.finish();

#ifndef NDEBUG
    std::cout << cmd_statistic << std::endl;

    for (const auto &statistic: pool_statistic) {
        std::cout << "поток(" << statistic.id << "), " << statistic << std::endl;
    }
#endif //#ifndef NDEBUG
}
