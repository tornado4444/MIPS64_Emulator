cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)

find_package(Threads REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")

add_executable(Rethink ${SOURCES}
	"src/BUS.cpp"
	"src/olc_64.cpp")
