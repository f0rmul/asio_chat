
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(third_party)
# Add source to this project's executable.
add_executable (asio_client "main.cpp" "src/client/client.cpp")

#protocol.hpp setup
add_library(protocol INTERFACE "src/protocol/protocol.hpp")
set_target_properties(protocol PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(protocol INTERFACE "src/protocol")

#client.hpp setup
add_library(client INTERFACE "src/client/client.cpp")
set_target_properties(client PROPERTIES LINKER_LANGUAGE CXX)
#target_include_directories(client INTERFACE "src/client")
target_link_libraries(client INTERFACE asio INTERFACE protocol)

 
target_link_libraries(asio_client client)