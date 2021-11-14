project(asio)

add_library(asio INTERFACE)
set_target_properties(asio PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(asio INTERFACE asio-lib/include)