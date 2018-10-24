#include <fstream>

#include "gtest/gtest.h"
#include "bulk/print.h"

TEST(print, log) {
    std::stringstream is{};

    std::vector<std::string> cmd_pool{"first", "second", "third"};
    auto                     result = print_log(cmd_pool, is);

    bool is_statistic_ok = ThreadPool::Statistic{{}, 1, 3} == result;
    EXPECT_TRUE(is_statistic_ok) << "Wrong statistic";

    EXPECT_EQ(is.str(), "Bulk: first, second, third\n");
}

TEST(print, file) {
    std::time_t              time = std::time(nullptr);
    std::vector<std::string> cmd_pool{"first", "second", "third"};
    size_t                   rnd  = 1;

    auto result          = print_file(time, rnd, cmd_pool);
    bool is_statistic_ok = ThreadPool::Statistic{{}, 1, 3} == result;
    EXPECT_TRUE(is_statistic_ok) << "Wrong statistic";

    std::ostringstream ss;
    ss << std::this_thread::get_id();
    std::string thread_id  = ss.str();

    std::fstream fs{};
    std::string  file_name = "bulk_num" + std::to_string(time) + "." + std::to_string(rnd) + "_" + thread_id + ".log";
    fs.open(file_name, std::ios::in);
    EXPECT_TRUE(fs.is_open()) << "Can't open file " << file_name;

    std::string str{};
    std::getline(fs, str);

    EXPECT_EQ(str, "Bulk: first, second, third");
}
