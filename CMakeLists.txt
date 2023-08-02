cmake_minimum_required(VERSION 3.18)

project(stdfuzz)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-fsanitize=fuzzer,address,undefined -fno-sanitize-recover=all -O1)
add_link_options(-fsanitize=fuzzer,address,undefined -fno-sanitize-recover=all)

#add_compile_options(-stdlib=libc++ -g)
#add_link_options(-stdlib=libc++)

add_executable(charconv charconv.cpp)
add_executable(regex regex.cpp)
