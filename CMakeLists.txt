cmake_minimum_required(VERSION 3.19)
project(BattleshipCinema)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(external/TermOx)
include_directories(external/SQLiteCpp/include)
add_executable(BattleshipCinema main.cpp MovieGraph.cpp MovieGraph.h Movie.h  mpreal.h parser.h)
target_link_libraries(BattleshipCinema TermOx mpfr)