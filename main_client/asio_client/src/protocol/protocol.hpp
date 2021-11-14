#pragma once

struct protocol
{
	static constexpr std::uint16_t MAX_PACKET_SIZE = 512;
	static constexpr std::uint16_t MAX_NICKNAME_SIZE = 16;
	static constexpr std::uint16_t PADDING = 24;
};