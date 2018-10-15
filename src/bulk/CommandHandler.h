#pragma once

#include <functional>
#include <vector>
#include <ctime>

struct CommandHandler {
    using command_pull_t = std::vector<std::string>;
    using fn_printer_t   = std::function<void(std::time_t, const command_pull_t &)>;

    /// Структура статискики.
    struct Statistic {
        size_t string_num;  ///< Количество строк.
        size_t bulk_num;    ///< Количество блоков.
        size_t command_num; ///< Количество команд.

        Statistic &operator+=(const Statistic &other) {
            string_num += other.string_num;
            bulk_num += other.bulk_num;
            command_num += other.command_num;
            return *this;
        }

        bool operator==(const Statistic &other) {
            return string_num == other.string_num &&
                   bulk_num == other.bulk_num &&
                   command_num == other.command_num;
        }
    };

    /**
     * Конструктор.
     * @param block_size Размер блока.
     */
    explicit CommandHandler(const std::size_t block_size);

    /**
     * Добавляет принтер.
     * @param printer Ссылка на функцию печати блока.
     * @return Идентификатор добавленной функции печати блока.
     */
    size_t add_printer(const fn_printer_t &printer);

    /**
     * Удаляет принтер
     * @param printer_id Идентификатор функции печати блока.
     */
    void del_printer(size_t printer_id);

    /**
     * Обрабатывает команду.
     * @param command Команда.
     */
    void add_command(const std::string &command);

    /// Выводит оставшиеся команды и выдает статистику.
    Statistic &finish();

    /// Деструктор.
    ~CommandHandler();

private:
    void print_pool();

private:
    size_t m_block_size{};
    size_t m_braces_num{};
    command_pull_t m_command_pool{};
    std::vector<fn_printer_t> m_printers{};
    std::time_t m_first_command_time{};
    Statistic m_statistic{};
};

