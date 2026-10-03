cmake_minimum_required(VERSION 4.1.2)
project(orderbook)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} main.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_compile_options(${PROJECT_NAME} PRIVATE 
  -Wall
  -Wextra
  -Wpedantic
  -Werror
)
