#pragma once
#include "Packet.h" //for Packet
#include <queue>	//for std::queue
#include <mutex>	//for std::mutex
#include <memory>	//for std::shared_ptr

class PacketManager
{
private:
	std::queue<std::shared_ptr<Packet>> m_packetsQueue;
	std::mutex m_packetsMutex;

public:

	void clear();
	bool hasPendingPackets();
	void append(std::shared_ptr<Packet> t_packet);

	std::shared_ptr<Packet> retrieve();
};