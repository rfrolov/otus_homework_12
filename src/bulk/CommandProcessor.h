#pragma once

#include "CommandHandler.h"
#include "ThreadPool.h"

class CmdProcessor {
public:

    /// Выдает ссылку на единожды созданный объект класса CmdProcessor.
    static CmdProcessor &getInstance() {
        static CmdProcessor instance{};
        return instance;
    }

    CmdProcessor(const CmdProcessor &) = delete;

    CmdProcessor(const CmdProcessor &&) = delete;

    CmdProcessor &operator=(const CmdProcessor &) = delete;

    CmdProcessor &operator=(const CmdProcessor &&) = delete;


    /**
     * Создает командный обработчик.
     * @param bulk Размер печатаемого блока.
     * @return Указатель на контекст обработчика.
     */
    void *create(std::size_t bulk);

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
        CommandHandler command_handler;
        std::string    remaining_data;
        std::size_t    print_log_id;
        std::size_t    print_file_id;
    };

    CmdProcessor() = default;

    ~CmdProcessor();

    std::mutex             m_mutex{};
    ThreadPool             m_thread_pool{std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1};
    std::vector<Context *> m_handlers{};
};