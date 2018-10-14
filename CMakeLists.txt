cmake_minimum_required(VERSION 3.6)

if (NOT DEFINED ENV{TRAVIS_BUILD_NUMBER})
    set(ENV{TRAVIS_BUILD_NUMBER} 1)
endif ()

find_package (Threads REQUIRED)

find_package(Boost REQUIRED)



add_subdirectory(src)
add_subdirectory(test)