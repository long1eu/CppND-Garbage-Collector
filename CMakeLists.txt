cmake_minimum_required(VERSION 3.14)
project(garbage_collector)

set(CMAKE_CXX_STANDARD 17)

add_executable(garbage_collector
        main.cpp
        include/gc_details.h
        include/gc_iterator.h
        include/gc_pointer.h
        LeakTester.h)