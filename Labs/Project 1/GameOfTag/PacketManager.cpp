#include "PacketManager.h"

void PacketManager::clear()
{
	std::lock_guard<std::mutex> lock(m_packetsMutex);
	m_packetsQueue = std::queue<std::shared_ptr<Packet>>{};
}

bool PacketManager::hasPendingPackets()
{
	std::lock_guard<std::mutex> lock(m_packetsMutex);
	return (!m_packetsQueue.empty());
}

void PacketManager::append(std::shared_ptr<Packet> t_packet)
{
	std::lock_guard<std::mutex> lock(m_packetsMutex);
	m_packetsQueue.push(std::move(t_packet));
}

std::shared_ptr<Packet> PacketManager::retrieve()
{
	std::lock_guard<std::mutex> lock(m_packetsMutex);
	std::shared_ptr<Packet> packet = m_packetsQueue.front();
	m_packetsQueue.pop();
	return packet;
}