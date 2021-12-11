#pragma once
#include "protocol.h"
#include "Server.h"

void SendPacket(void* buff)
{
	char* packet = reinterpret_cast<char*> (buff);
	int packet_size = MAKEWORD(packet[0], packet[1]);

	for (auto& cl : Client_sock)
		send(cl, packet, packet_size, 0);
}

void SendLoginOkPacket(char id)
{
	sc_packet_login_ok packet;
	packet.id = id;
	packet.size = sizeof(packet);
	packet.type = SC_LOGIN_OK;
	
	send(Client_sock[id], (char*)&packet, sizeof(packet), 0);
}

void SendReadyPacket(SOCKET sock, char other)
{
	sc_packet_ready packet;
	packet.id = other;
	packet.size = sizeof(packet);
	packet.type = SC_READY;
	packet.pt = ri[other].pt_player;
	packet.ready = ri[other].is_ready; 
	send(sock, (char*)&packet, sizeof(packet), 0);
}

void SendGameStartPacket()
{
	std::cout << "Send Game STart!" << std::endl;

	sc_packet_game_start packet;
	packet.size = sizeof(packet);
	packet.type = SC_GAMESTART;
	packet.clients_num = count_s+1;

	SendPacket(&packet);//��� Ŭ�󿡰� ����
}

void SendPlayerPosPacket(Player_pos& ps)
{
	sc_packet_player_pos packet;
	packet.size = sizeof(packet);
	packet.type = SC_PLAYER_POS;
	memcpy(&packet.players, &ps, sizeof(packet.players));

	SendPacket(&packet);//��� Ŭ�󿡰� ����
}

void SendBulletPosPacket(Bullet_pos& bp)
{
	sc_packet_bullet_pos packet;
	packet.size = sizeof(packet);
	packet.type = SC_BULLET_POS;
	memcpy(&packet.bullets, &bp, sizeof(packet.bullets));

	SendPacket(&packet);//��� Ŭ�󿡰� ����
}

void SendCubePosPacket(Cube_pos& cp)
{
	sc_packet_cube_pos packet;
	packet.size = sizeof(packet);
	packet.type = SC_MAP_CUBE;
	memcpy(&packet.cubes, &cp, sizeof(packet.cubes));

	SendPacket(&packet);//��� Ŭ�󿡰� ����
}

void SendRemovePlayerPacket(char client, char leaver)
{
	sc_packet_remove_player packet;
	packet.id = leaver;
	packet.size = sizeof(packet);
	packet.type = SC_REMOVE_PLAYER;
	SendPacket(&packet);
}