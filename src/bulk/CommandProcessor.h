#pragma once

#include "CommandHandler.h"
#include "ThreadPool.h"

class CmdProcessor {
public:

    using context_t = void*;

    /// Выдает ссылку на единожды созданный объект класса CmdProcessor.
    static CmdProcessor &getInstance() {
        static CmdProcessor instance{};
        return instance;
    }

    CmdProcessor(const CmdProcessor &) = delete;

    CmdProcessor(const CmdProcessor &&) = delete;

    CmdProcessor &operator=(const CmdProcessor &) = delete;

    CmdProcessor &operator=(const CmdProcessor &&) = delete;

    void set_block_size(std::size_t block_size);

    /**
     * Создает командный обработчик.
     * @return Контекст обработчика.
     */
    context_t create();

    /**
     * Обрабатывает данные конкретного обработчика.
     * @param handle Указатель на контекст обработчика.
     * @param new_data Данные для обработки.
     */
    void process(void *handle, const std::string &new_data);

    /**
     * Удаляет командный обработчик и выводит по нему статистику.
     * @param handle Указатель на контекст обработчика.
     */
    void destroy(void *handle);

private:
    struct Context {
        std::string remaining_data{};
    };

    CmdProcessor() = default;

    ~CmdProcessor();

    std::mutex m_mutex{};
    ThreadPool m_thread_pool{std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1};
    std::vector<std::unique_ptr<Context>> m_handles{};
    CommandHandler m_command_handler{1};
    std::size_t m_print_log_id{};
    std::size_t m_print_file_id{};
};