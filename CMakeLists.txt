cmake_minimum_required(VERSION 3.21)
project(mem C)

set(CMAKE_C_STANDARD 99)

add_executable(mem mymem.c linked_list.h linked_list.c memorytests.c)