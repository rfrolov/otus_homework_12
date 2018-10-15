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

void *CmdProcessor::create(std::size_t bulk) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_handlers.emplace_back(new Context{CommandHandler{bulk}, std::string{}});

    auto *handle = m_handlers.back();
    auto &command_handler = handle->command_handler;
    handle->print_log_id = command_handler.add_printer(
            [this](std::time_t, CommandHandler::command_pull_t command_pull) {
                m_thread_pool.enqueue([command_pull] { return print_log(command_pull); });
            });

    handle->print_file_id = command_handler.add_printer(
            [this](std::time_t time, CommandHandler::command_pull_t command_pull) {
                static size_t rnd{};
                m_thread_pool.enqueue([time, command_pull] { return print_file(time, rnd++, command_pull); });
            });
    return handle;
}

void CmdProcessor::process(void *handle, const std::string &new_data) {
    std::unique_lock<std::mutex> lock(m_mutex);

    auto it = std::find(m_handlers.cbegin(), m_handlers.cend(), handle);
    if (it == m_handlers.cend()) { return; }

    auto &context = *it;
    auto &command_handler = context->command_handler;

    std::string data = context->remaining_data + new_data;
    std::size_t data_size = data.length();

    std::size_t last_index{};
    for (std::size_t index{0}; index < data_size; ++index) {
        if (data[index] == '\n') {
            command_handler.add_command(std::string(&data[last_index], &data[index]));
            last_index = index + 1;
        }
    }
    context->remaining_data = last_index < data_size ? std::string(&data[last_index], &data[data_size]) : "";
}

void CmdProcessor::destroy(void *handle) {
    std::unique_lock<std::mutex> lock(m_mutex);

    auto it = std::find(m_handlers.cbegin(), m_handlers.cend(), handle);
    if (it == m_handlers.cend()) { return; }

    auto &context = *it;
    auto &command_handler = context->command_handler;

    auto cmd_statistic = command_handler.finish();
    command_handler.del_printer(context->print_log_id);
    command_handler.del_printer(context->print_file_id);

#ifndef NDEBUG
    std::cout << "handle (" << context << "): " << cmd_statistic << std::endl;
#endif //#ifndef NDEBUG

    delete *it;
    m_handlers.erase(it);
}


CmdProcessor::~CmdProcessor() {
    auto pool_statistic = m_thread_pool.finish();

#ifndef NDEBUG
    for (const auto &statistic: pool_statistic) {
        std::cout << "поток(" << (&statistic - &pool_statistic[0] + 1) << "), " << statistic << std::endl;
    }
#endif //#ifndef NDEBUG
}
