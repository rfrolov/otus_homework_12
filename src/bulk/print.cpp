#include "print.h"
#include <fstream>
#include <sstream>

template<typename T>
ThreadPool::Statistic print(const std::vector<std::string> &command_pool, T &stream) {
    stream << "Bulk: ";
    for (const auto &command : command_pool) {
        stream << command << (&command != &command_pool.back() ? ", " : "");
    }
    stream << std::endl;
    return ThreadPool::Statistic{{}, 1, command_pool.size()};
}

ThreadPool::Statistic print_log(const std::vector<std::string> &command_pool, std::ostream &os) {
    return print(command_pool, os);
}

ThreadPool::Statistic print_file(std::time_t time, size_t rnd, const std::vector<std::string> &command_pool) {
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    std::string thread_id  = ss.str();

    std::fstream fs{};
    std::string  file_name = "bulk_num" + std::to_string(time) + "." + std::to_string(rnd) + "_" + thread_id + ".log";

    fs.open(file_name, std::ios::out);
    if (!fs.is_open()) { return ThreadPool::Statistic{{}, 0, 0}; }

    auto result = print(command_pool, fs);
    fs.close();
    return result;
}
