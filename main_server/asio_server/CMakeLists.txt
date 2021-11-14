
cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

add_subdirectory(third_party)

add_executable(server_exe "main.cpp")

#protocol.hpp setup
add_library(protocol INTERFACE "src/protocol/protocol.hpp")
set_target_properties(protocol PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(protocol INTERFACE "src/protocol")

#chat_room.hpp setup
add_library(chat_room INTERFACE "src/chat_room/chat_room.hpp")
set_target_properties(chat_room PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(chat_room INTERFACE protocol)

#session.hpp setup
add_library(session INTERFACE "src/session/session.hpp")
set_target_properties(session PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(session INTERFACE asio INTERFACE spdlog)
target_link_libraries(session INTERFACE chat_room)

#generic_server.hpp setup
add_library(generic_server INTERFACE "generic_server.hpp")
set_target_properties(generic_server PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(generic_server INTERFACE session)


target_link_libraries(server_exe generic_server)



