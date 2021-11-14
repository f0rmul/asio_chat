#pragma once
#include "../protocol/protocol.hpp"

#include <array>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <algorithm>

#pragma warning(disable : 4996)

class participant
{
public:
	virtual ~participant() {}
	virtual void deliver_message(std::array<char, protocol::MAX_PACKET_SIZE>&) = 0;
};


class chat_room final
{
	using shared_participant_ptr = std::shared_ptr<participant>;
	using message = std::array<char, protocol::MAX_PACKET_SIZE>;
public:

	void join(shared_participant_ptr, const std::string&);
	void leave(shared_participant_ptr);
	void broadcast(message&, shared_participant_ptr);
	std::string get_nickname(shared_participant_ptr);

private:

	static constexpr int max_recent_messages = 100;
	std::unordered_set< shared_participant_ptr> m_participants;
	std::unordered_map< shared_participant_ptr, std::string> m_name_table;
	std::deque<message> m_message_queue;
};


void chat_room::join(shared_participant_ptr participant, const std::string& nickname)
{
	m_participants.insert(participant);
	m_name_table[participant] = nickname;

	std::for_each(m_message_queue.begin(), m_message_queue.end(),
		[&participant](message& msg)
		{
			participant->deliver_message(msg);
		});
}

void chat_room::leave(shared_participant_ptr participant)
{
	m_participants.erase(participant);
	m_name_table.erase(participant);
}

void chat_room::broadcast(message& msg, shared_participant_ptr participant)
{
	std::string nickname = get_nickname(participant);
	message formatted_message;

	strcpy(formatted_message.data(), nickname.c_str());
	strcat(formatted_message.data(), msg.data());


	m_message_queue.push_back(formatted_message);

	while (m_message_queue.size() > max_recent_messages)
		m_message_queue.pop_front();

	std::for_each(m_participants.begin(), m_participants.end(),
		[&formatted_message](shared_participant_ptr participant)
		{
			participant->deliver_message(formatted_message);
		});
}

std::string chat_room::get_nickname(shared_participant_ptr participant)
{
	return m_name_table.at(participant);
}