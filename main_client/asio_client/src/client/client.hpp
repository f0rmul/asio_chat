#pragma once 
#include <asio.hpp>
#include <thread>
#include <array>
#include <deque>
#include <algorithm>
#include <iostream>

#include "../protocol/protocol.hpp"
#pragma warning(disable : 4996)

class asio_client final
{
	using err_code = asio::error_code;
	using nickname_buff = std::array<char, protocol::MAX_NICKNAME_SIZE>;
	using message_buff = std::array<char, protocol::MAX_PACKET_SIZE>;

public:
	asio_client(const nickname_buff& nickname, asio::io_context& context, const std::string& remote_host) : m_context{ context }, m_socket{ m_context }
	{

		strcpy(m_nickname.data(), nickname.data());
		memset(m_read_message.data(), '\0', protocol::MAX_PACKET_SIZE);
		asio::ip::tcp::endpoint end{ asio::ip::address::from_string(remote_host),1234 };

		get_socket().async_connect(end, std::bind(&asio_client::connection_handler, this, std::placeholders::_1));
	}

	asio::ip::tcp::socket& get_socket() { return m_socket; }

	void abort() { m_context.post([=]() {m_socket.close(); }); }

	void write(const message_buff&);
private:

	/* HANDLERS */

	void send_packet(const  message_buff&);

	void connection_handler(const err_code&);

	void read_packet_handler(const  err_code&);

	void send_packet_handler(const  err_code&);

	/* ******** */


	nickname_buff m_nickname;
	asio::io_context& m_context;
	message_buff m_read_message;
	asio::ip::tcp::socket m_socket;
	std::deque<message_buff> m_message_queue;
};