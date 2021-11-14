
project(asio)
project(spdlog)

add_library(asio INTERFACE)
set_target_properties(asio PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(asio INTERFACE asio-lib/include)

add_library(spdlog INTERFACE)
set_target_properties(spdlog PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(spdlog INTERFACE spdlog/include)