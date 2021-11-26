#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <algorithm>

#include "Server.h"
#include "protocol.h"
#include "Cube.h"
using namespace std;

#define SERVERPORT 9000
#define BUFSIZE 1024
#define MAXPLAYER 3

int GetSize;
char Buffer[BUFSIZE];




DWORD WINAPI recv_thread(LPVOID arg);
//DWORD WINAPI Calcutlaion_Thread(LPVOID arg);
void Calcutlaion_clients();

//�����Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

//�����Լ� ���� ���
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

//����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}
	return (len - left);
}


int count_s = 0;
int main()
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	//���� ����
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");

	//bind
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");

	//listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	//������ ��ſ� ����� ����
	SOCKET client_sock;

	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	ZeroMemory(&clientaddr, addrlen);

	char buf[BUFSIZE + 1];

	bool check_ready[MAXPLAYER];
	int clients_count = 0;
	int ready_count = 0;
	bool changestate = true;
	int user_index = -1;

	HANDLE hThread;
	ready_info rf_Packet;
	while (1)
	{
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}
		if (clients_count == MAXPLAYER) { //�ִ� Ŭ�� ���ӽ� �������
			closesocket(client_sock);
			continue;
		}
		Client_sock[count_s] = client_sock;
		count_s++;

		//recv ������ ����
		hThread = CreateThread(NULL, 0, recv_thread, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) {
			closesocket(client_sock);
		}
		else {
			CloseHandle(hThread);
		}
	}
	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;

}

DWORD WINAPI recv_thread(LPVOID arg) {
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;
	int len;
	int user_index = -1;
	char buf[BUFSIZE + 1];

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
	for (int i = 0; i < MAXPLAYER; ++i) {
		if (false == connectedCls[i].is_connected) {
			connectedCls[i].is_connected = true;
			user_index = i;
			cout << "==============================" << endl;
			cout << "ID " << i << " is connected" << endl;
			cout << "==============================" << endl;

			break;
		}
	}
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Ŭ�󿡰� ���� id ����
	char user_index_c[10];
	itoa(user_index, user_index_c, 10);
	len = sizeof(user_index_c);
	retval = send(client_sock, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	// ������ ������
	retval = send(client_sock, (char*)&user_index_c, sizeof(user_index_c), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		//exit( 1 );
	}
	printf("������ ����");

	//--------------------------------------------------------------------------------
	//���� ���� ���� �ޱ�

	cout << "����1" << endl;
	retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}
	else if (retval == 0) {
	}
	GetSize = recv(client_sock, Buffer, len, 0);
	//Buffer[GetSize] = '\0';
	ri = (ready_info*)Buffer;
	ari.is_ready[ri->id] = ri->is_ready;
	ari.Pt_Players[ri->id] = ri->pt_player;
	//---------------------------------------------------------------------------------
	cout << "����2" << endl;

	Message msg{};
	cout << "����3" << endl;
	while (true) {

		if (!ari.game_start) { //���� �����Ҷ�����
			while (1) {
				cout << "����4" << endl;
				//�������� ��� Ŭ�� ready ���� ����
				len = sizeof(ari);
				for (int i = 0; i < count_s; i++) {
					retval = send(Client_sock[i], (char*)&len, sizeof(int), 0);
					if (retval == SOCKET_ERROR) {
						err_display("send()");
					}

					// ������ ������
					retval = send(Client_sock[i], (char*)&ari, len, 0);
					if (retval == SOCKET_ERROR) {
						err_display("send()");
						//exit( 1 );
					}
				}
				cout << "����5" << endl;
				//�������� ready ���� ���� 
				retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
				if (retval == SOCKET_ERROR) {
					err_display("recv()");
				}
				else if (retval == 0) {
				}
				GetSize = recv(client_sock, Buffer, len, 0);
				Buffer[GetSize] = '\0';
				ri = (ready_info*)Buffer;
				ari.is_ready[ri->id] = ri->is_ready;
				ari.Pt_Players[ri->id] = ri->pt_player;

				cout << "����6" << endl;
				if (count_s == 1 && ari.is_ready[0]) {
					ari.game_start = true;
					thread PhysicsThread(Calcutlaion_clients);
					PhysicsThread.detach();
					break;
				}
				if (count_s == 2 && ari.is_ready[0] && ari.is_ready[1]) {
					ari.game_start = true;
					thread PhysicsThread(Calcutlaion_clients);
					PhysicsThread.detach();
					break;
				}
				if (count_s == 3 && ari.is_ready[0] && ari.is_ready[1] && ari.is_ready[2]) {
					ari.game_start = true;
					thread PhysicsThread(Calcutlaion_clients);
					PhysicsThread.detach();
					break;
				}
			}
		}

		if (ari.game_start) { //���� ���� 
			std::cout << "physics thread ����!" << std::endl;


			//CreateThread(NULL, 0, Calcutlaion_Thread, NULL, 0, NULL); //���ڷ� �� �ѱ��?
			while (ari.game_start) {
				char retval = recv(client_sock, buf, 1, 0);

				// Ŭ���̾�Ʈ ���� ����, recv ���� ó��
				if (retval == 0 || retval == SOCKET_ERROR) {
					closesocket(client_sock);
				}
				switch ((int)buf[0])
				{
				case CS_PLAYER_LEFT:
				{
					cout << "����" << endl;
					msg.id = user_index;
					msg.type = TYPE_PLAYER;
					msg.dir = DIR_LEFT;
					msg.isPushed = true;
					break;
				}

				}

				if (msg.id != -1)
				{
					glo_MsgQueue.emplace(msg);
				}
			}
		}
	}
	//---------------------------------------------------------------------------------------------
	closesocket(client_sock);
	return 0;
}

void Calcutlaion_clients() {
	cout << "����3" << endl;
	std::queue <Message> MsgQueue;
	Message Msg;
	Player_data player_data[3];
	int retval;

	while (true) {
		MsgQueue = glo_MsgQueue;

		while (!glo_MsgQueue.empty())
			glo_MsgQueue.pop();

		while (!MsgQueue.empty()) {
			Msg = MsgQueue.front();
			MsgQueue.pop();
			if (Msg.type == TYPE_PLAYER) {
				switch (Msg.dir)
				{
					/*	case DIR_UP:
							phyPlayers[phyMsg.id].SetKeyW(phyMsg.isPushed);
							break;
						case DIR_DOWN:
							phyPlayers[phyMsg.id].SetKeyS(phyMsg.isPushed);
							break;*/
				case DIR_LEFT:
					cout << "����7" << endl;
					player_data->rotate += 2.0f * player_data->speed;
					break;
					/*case DIR_RIGHT:
						phyPlayers[phyMsg.id].SetKeyD(phyMsg.isPushed);
						break;*/
				}
			}

			if (Msg.type == TYPE_BULLET) {
				break;
			}
		}

		int len = sizeof(player_data);

		for (int i = 0; i < count_s; i++) {
			retval = send(Client_sock[i], (char*)&len, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");

			}
			retval = send(Client_sock[i], (char*)&player_data, sizeof(player_data), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");

			}
		}

		// ������, ���Ĺ� Ŭ���̾�Ʈ z��

		float cmpZ[3];
		for (int i = 0; i < count_s; ++i)
			cmpZ[i] = player_data[i].PosVec_z;
		if (count_s == 1)
		{
			LastPosZ = cmpZ[0];
			FirstPosZ = cmpZ[0];

			printf("%f\n", LastPosZ);
		}

		if (count_s == 2)
		{
			LastPosZ = min(cmpZ[0], cmpZ[1]);
			FirstPosZ = max(cmpZ[0], cmpZ[1]);
		}
		if (count_s == 3)
		{
			LastPosZ = min(cmpZ[0], cmpZ[1], cmpZ[2]);
			FirstPosZ = max(cmpZ[0], cmpZ[1], cmp[2]);
		}
		ZeroMemory(&CubeList, sizeof(Cube_data) * 10);

		//���� �˻��ؼ� ť�� �����ϰ� CubeList�� Cube ����
		if ((int)FirstPosZ % 100 == 0 && FirstPosZ > 100.0f)
		{
			Cube_data c;
			c.Life = rand() % 3;
			c.zOffset = FirstPosZ + 300.0f + rand() % 100;
			c.rotate = rand() % 360;
			CubeList[CubeCounter] = c;
			//printf("%f\n", c.zOffset);
			if (CubeCounter < 10-1)
				CubeCounter += 1;
		}

		len = sizeof(CubeList);

		for (int i = 0; i < count_s; ++i)
		{
			retval = send(Client_sock[i], (char*)&len, sizeof(int), 0);
			retval = send(Client_sock[i], (char*)CubeList, len, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}

		}
	}
}

