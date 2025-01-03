cmake_minimum_required(VERSION 3.14)

project(SimpleShell VERSION 1.0 DESCRIPTION "SimpleShell" LANGUAGES C)

include_directories(include)
include_directories(src)
file(GLOB SOURCES_C "src/shell.c")
file(GLOB SOURCES_H "include/msgs.h")

add_executable(shell ${SOURCES_C} ${SOURCES_H})
