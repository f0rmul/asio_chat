#pragma once 
#include "client.hpp"

void asio_client::write(const message_buff& message)
{
	m_context.post(bind(&asio_client::send_packet, this, message));
}

void asio_client::connection_handler(const err_code& error)
{
	if (!error)
	{
		std::cout << "Connected to :" << get_socket().remote_endpoint().address().to_string() << std::endl;
		asio::async_write(get_socket(),
			asio::buffer(m_nickname.data(), m_nickname.size()),
			std::bind(&asio_client::read_packet_handler, this, std::placeholders::_1));
	}
	else
	{
		std::cout << "Cant connect to the server..." << std::endl;
	}
}

void asio_client::read_packet_handler(const  err_code& error)
{
	std::cout << m_read_message.data() << std::endl;
	if (!error)
	{
		asio::async_read(get_socket(),
			asio::buffer(m_read_message.data(), m_read_message.size()),
			std::bind(&asio_client::read_packet_handler, this, std::placeholders::_1));
	}
	else
	{
		get_socket().close();
	}
}

void asio_client::send_packet(const  message_buff& message)
{
	bool write_in_progress = !m_message_queue.empty();
	m_message_queue.push_back(message);
	if (!write_in_progress)
	{
		asio::async_write(get_socket(),
			asio::buffer(m_message_queue.front().data(), m_message_queue.front().size()),
			std::bind(&asio_client::send_packet_handler, this, std::placeholders::_1));
	}
}

void asio_client::send_packet_handler(const  err_code& error)
{
	if (!error)
	{
		m_message_queue.pop_front();
		if (!m_message_queue.empty())
		{
			asio::async_write(get_socket(),
				asio::buffer(m_message_queue.front().data(), m_message_queue.front().size()),
				std::bind(&asio_client::send_packet_handler, this, std::placeholders::_1));
		}
	}
	else
	{
		get_socket().close();
	}
}