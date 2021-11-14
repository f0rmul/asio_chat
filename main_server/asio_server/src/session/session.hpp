#pragma once

#include <asio.hpp>
#include <spdlog/spdlog.h>
#include "../chat_room/chat_room.hpp"

#include <array>
#include <deque>
#include <memory>
#include <iostream>

#pragma warning(disable : 4996)

class connection_handler final : public participant, public std::enable_shared_from_this<connection_handler>
{
	using err_code = asio::error_code;
	using nickname_buff = std::array<char, protocol::MAX_NICKNAME_SIZE>;
	using message_buff = std::array<char, protocol::MAX_PACKET_SIZE>;
public:

	explicit connection_handler(asio::io_context& context, asio::io_context::strand& strand, chat_room& room) :m_socket{ context }, m_strand{ strand }, m_room{ room }
	{}
	asio::ip::tcp::socket& get_socket() & { return m_socket; }

	void start();

	void deliver_message(message_buff&) override;
private:
	/* HANDLERS */
	void process_nickname(const err_code&, std::size_t);

	void read_packet_handler(const err_code&, std::size_t);

	void write_packet_handler(const err_code&, std::size_t);
private:
	asio::ip::tcp::socket m_socket;
	asio::io_context::strand& m_strand;
	chat_room& m_room;

	nickname_buff m_nickname;
	message_buff m_message;
	std::deque<message_buff> m_message_queue;
};


void connection_handler::start()
{
	asio::async_read(get_socket(), asio::buffer(m_nickname.data(), m_nickname.size()), m_strand.wrap(
		[m_shared = shared_from_this()](const err_code& error, std::size_t bytes_transfered)
	{
		m_shared->process_nickname(error, bytes_transfered);
	}));
}

void connection_handler::deliver_message(message_buff& message)
{

	bool write_in_progress = !m_message_queue.empty();
	m_message_queue.push_back(message);
	if (!write_in_progress)
	{
		asio::async_write(get_socket(), asio::buffer(m_message_queue.front().data(), m_message_queue.front().size()), m_strand.wrap(
			[m_shared = shared_from_this()](const err_code& error, std::size_t bytes_transfered)
		{
			m_shared->write_packet_handler(error, bytes_transfered);
		}));
	}
}


void connection_handler::process_nickname(const err_code& error, std::size_t bytes_transfered)
{
	if (error)
	{
		spdlog::error("Errorr in process_nickname() : " + error.message());
		return;
	}
	if (strlen(m_nickname.data()) <= protocol::MAX_NICKNAME_SIZE - 2)
	{
		strcat(m_nickname.data(), ": ");
	}
	else
	{
		m_nickname[protocol::MAX_NICKNAME_SIZE - 2] = ':';
		m_nickname[protocol::MAX_NICKNAME_SIZE - 1] = ' ';
	}
	m_room.join(shared_from_this(), std::string(m_nickname.data()));

	asio::async_read(get_socket(), asio::buffer(m_message.data(), m_message.size()),
		m_strand.wrap([m_shared = shared_from_this()](const err_code& error, std::size_t bytes_transfered)
	{
		m_shared->read_packet_handler(error, bytes_transfered);
	}));
}

void connection_handler::read_packet_handler(const err_code& error, std::size_t bytes_transfered)
{
	if (error)
	{
		spdlog::error("Error in read_packet_handler() : " + error.message());

		m_room.leave(shared_from_this());
		return;
	}

	m_room.broadcast(m_message, shared_from_this());

	asio::async_read(get_socket(), asio::buffer(m_message.data(), m_message.size()),
		m_strand.wrap([m_shared = shared_from_this()](const err_code& error, std::size_t bytes_transfered)
	{
		m_shared->read_packet_handler(error, bytes_transfered);
	}));
}


void connection_handler::write_packet_handler(const err_code& error, std::size_t bytes_transfered)
{
	if (error)
	{
		spdlog::error("Error in write_packet_handler() : " + error.message());
		m_room.leave(shared_from_this());
		return;
	}

	m_message_queue.pop_front();
	if (!m_message_queue.empty())
	{
		asio::async_write(get_socket(), asio::buffer(m_message_queue.front().data(), m_message_queue.front().size()),
			m_strand.wrap([m_shared = shared_from_this()](const err_code& error, std::size_t bytes_transfered)
		{
			m_shared->write_packet_handler(error, bytes_transfered);
		}));
	}
}