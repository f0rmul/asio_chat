#pragma once
#include "src/session/session.hpp"

#include <memory>
#include <vector>
#include <thread>



template <typename connection_handler>
class asio_server final
{

	using shared_handler_t = std::shared_ptr<connection_handler>;
	using err_code = asio::error_code;
public:

	asio_server() = default;

	asio_server(const asio_server&) = delete;
	asio_server& operator=(const asio_server&) = delete;

	asio_server(asio_server&&) = delete;
	asio_server& operator=(asio_server&&) = delete;

	explicit asio_server(const std::size_t thread_count = 1) :m_strand{ m_context }, m_acceptor{ m_context }, m_threads_count{ thread_count }
	{
		m_thread_pool.reserve(m_threads_count);
	}
	~asio_server();

	void start_server(const uint16_t port);

private:

	void run();
	void handle_new_connection(shared_handler_t, const err_code&);

	asio::io_context m_context;
	asio::io_context::strand m_strand;
	asio::ip::tcp::acceptor m_acceptor;
	std::vector<std::thread> m_thread_pool;
	std::size_t m_threads_count;
	chat_room m_room;
};

template <typename connection_handler>
void asio_server<connection_handler>::start_server(const uint16_t port)
{
	spdlog::info("SERVER STARTED");

	/*setup for acceptor*/
	asio::ip::tcp::endpoint end_point(asio::ip::tcp::v4(), port);
	m_acceptor.open(end_point.protocol());
	asio::socket_base::reuse_address option_reuse(true);
	m_acceptor.set_option(option_reuse);
	m_acceptor.bind(end_point);
	m_acceptor.listen();

	/*start accepting*/
	run();

	for (std::size_t i = 0; i < m_threads_count; ++i)
		m_thread_pool.emplace_back([=] {m_context.run(); });
}

template <typename connection_handler>
void asio_server<connection_handler>::run()
{
	auto participant = std::make_shared<connection_handler>(m_context, m_strand, m_room);
	m_acceptor.async_accept(participant->get_socket(), m_strand.wrap(
		[=](const err_code& error)
		{
			handle_new_connection(participant, error);
		}));
}

template <typename connection_handler>
void asio_server<connection_handler>::handle_new_connection(shared_handler_t new_participant, const err_code& error)
{
	if (!error)
	{
		spdlog::info("New user : " + (new_participant->get_socket().remote_endpoint().address().to_string()));
		new_participant->start();
	}
	run();
}

template <typename connection_handler>
asio_server<connection_handler>::~asio_server()
{
	try
	{
		for (auto&& el : m_thread_pool)
			el.join();

		m_acceptor.close();
		m_context.stop();

		spdlog::info("SERVER WAS STOPED");
	}
	catch (const std::system_error& se)
	{
		spdlog::error(se.what());
	}
	catch (...)
	{
		spdlog::error("Unknown error in ~asio_server()");
	}
}