#include "generic_server.hpp"

int main()
{
    try
    {
        auto server = std::make_unique<asio_server<connection_handler>>(std::thread::hardware_concurrency()); // amount of threads
        server->start_server(1234); // port
    }
    catch (const std::exception& e)
    {
        spdlog::error(e.what());
    }
}
