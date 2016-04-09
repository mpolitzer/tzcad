cmake_minimum_required(VERSION 3.2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -O2 -Wall -pedantic -std=c99 -g -I../include")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")

include(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(GL   REQUIRED gl)
add_executable(tzeda main.c tz/tz/gl/gl_core_3_3.c)
target_link_libraries(tzeda SDL2 GL m dl)

