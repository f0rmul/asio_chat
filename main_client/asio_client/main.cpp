
#include "src/client/client.hpp"
int main()
{
	try
	{
		asio::io_context context;
		std::array<char, protocol::MAX_NICKNAME_SIZE> nickname_buff;
		std::string nickname;

		std::cout << "Enter your nick here : ";
		std::getline(std::cin, nickname);
		strcpy(nickname_buff.data(), nickname.c_str());

		auto client = std::make_unique<asio_client>(nickname_buff, context, "192.168.1.42"); //remote host

		std::thread th([&context]() {context.run(); });

		std::array<char, protocol::MAX_PACKET_SIZE> message;

		while (true)
		{
			memset(message.data(), '\0', message.size());
			if (!std::cin.getline(message.data(), protocol::MAX_PACKET_SIZE - protocol::PADDING - protocol::MAX_NICKNAME_SIZE))
			{
				std::cin.clear();
			}
			client->write(message);
		}

		client->abort();
		th.join();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}