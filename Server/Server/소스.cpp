#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "Server.h"
#include "protocol.h"
using namespace std;

#define SERVERPORT 9000
#define BUFSIZE 1024
#define MAXPLAYER 3

int GetSize;
char Buffer[BUFSIZE];


DWORD WINAPI recv_thread(LPVOID arg);
//DWORD WINAPI Calcutlaion_Thread(LPVOID arg);
void Calcutlaion_clients();
void init_player(Col_Player_data cpd);
//void update_player(Col_Player_data cpd);
//void update_time();
//void update_camera(Col_Player_data cpd, Col_Camera_data ccd);
//void init_camera(Col_Camera_data ccd);
//소켓함수 오류 출력 후 종료
Game_data game_data;
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

//소켓함수 오류 출력
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

//사용자 정의 데이터 수신 함수
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

	//소켓 생성
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

	//데이터 통신에 사용할 변수
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
	while (1)
	{
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}
		if (clients_count == MAXPLAYER) { //최대 클라 접속시 끊어버림
			closesocket(client_sock);
			continue;
		}
		Client_sock[count_s] = client_sock;
		count_s++;

		//recv 스레드 생성
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

	// 윈속 종료
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

	// 클라이언트 정보 얻기
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
	//클라에게 유저 id 전송
	char user_index_c[10];
	itoa(user_index, user_index_c, 10);
	len = sizeof(user_index_c);
	retval = send(client_sock, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	// 데이터 보내기
	retval = send(client_sock, (char*)&user_index_c, sizeof(user_index_c), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		//exit( 1 );
	}
	printf("데이터 전송");

	//--------------------------------------------------------------------------------
	//참가 유저 정보 받기

	cout << "여기1" << endl;
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
	cout << "여기2" << endl;

	Message msg{};
	cout << "여기3" << endl;
	while (true) {

		if (!ari.game_start) { //게임 시작할때까지
			while (1) {
				cout << "여기4" << endl;
				//유저에게 모든 클라 ready 상태 전송
				len = sizeof(ari);
				for (int i = 0; i < count_s; i++) {
					retval = send(Client_sock[i], (char*)&len, sizeof(int), 0);
					if (retval == SOCKET_ERROR) {
						err_display("send()");

					}

					// 데이터 보내기
					retval = send(Client_sock[i], (char*)&ari, len, 0);
					if (retval == SOCKET_ERROR) {
						err_display("send()");
						//exit( 1 );
					}
				}
				cout << "여기5" << endl;
				//유저에게 ready 상태 받음 
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

				cout << "여기6" << endl;
				if (count_s == 1 && ari.is_ready[0]) {
					ari.pt_clients_num = 1; //최종 참가하는 클라수 
					ari.game_start = true;

					len = sizeof(ari);
					for (int i = 0; i < count_s; i++) {
						retval = send(Client_sock[i], (char*)&len, sizeof(int), 0);
						if (retval == SOCKET_ERROR) {
							err_display("send()");

						}

						// 데이터 보내기
						retval = send(Client_sock[i], (char*)&ari, len, 0);
						if (retval == SOCKET_ERROR) {
							err_display("send()");
							//exit( 1 );
						}
					}

					thread PhysicsThread(Calcutlaion_clients);
					PhysicsThread.detach();
					break;
				}
				if (count_s == 2 && ari.is_ready[0] && ari.is_ready[1]) {
					ari.pt_clients_num = 2; //최종 참가하는 클라수 
					ari.game_start = true;

					len = sizeof(ari);
					for (int i = 0; i < count_s; i++) {
						retval = send(Client_sock[i], (char*)&len, sizeof(int), 0);
						if (retval == SOCKET_ERROR) {
							err_display("send()");

						}

						// 데이터 보내기
						retval = send(Client_sock[i], (char*)&ari, len, 0);
						if (retval == SOCKET_ERROR) {
							err_display("send()");
							//exit( 1 );
						}
					}

					thread PhysicsThread(Calcutlaion_clients);
					PhysicsThread.detach();
					break;
				}
				if (count_s == 3 && ari.is_ready[0] && ari.is_ready[1] && ari.is_ready[2]) {
					ari.pt_clients_num = 3; //최종 참가하는 클라수 
					ari.game_start = true;

					len = sizeof(ari);
					for (int i = 0; i < count_s; i++) {
						retval = send(Client_sock[i], (char*)&len, sizeof(int), 0);
						if (retval == SOCKET_ERROR) {
							err_display("send()");

						}

						// 데이터 보내기
						retval = send(Client_sock[i], (char*)&ari, len, 0);
						if (retval == SOCKET_ERROR) {
							err_display("send()");
							//exit( 1 );
						}
					}

					thread PhysicsThread(Calcutlaion_clients);
					PhysicsThread.detach();
					break;
				}
			}
		}

		if (ari.game_start) { //게임 시작 
			std::cout << "physics thread 생성!" << std::endl;
			QueryPerformanceFrequency(&tSecond);
			QueryPerformanceCounter(&tTime);

			//CreateThread(NULL, 0, Calcutlaion_Thread, NULL, 0, NULL); //인자로 뭘 넘길까?
			while (ari.game_start) {
				char retval = recv(client_sock, buf, 1, 0);

				// 클라이언트 접속 종료, recv 에러 처리
				if (retval == 0 || retval == SOCKET_ERROR) {
					closesocket(client_sock);
				}
				switch ((int)buf[0])
				{
				case SC_PLAYER_LEFT_DOWN:
				{
					msg.id = user_index;
					msg.type = TYPE_PLAYER;
					msg.dir = DIR_LEFT_GO;
					msg.isPushed = true;
					break;
				}
				case SC_PLAYER_LEFT_UP:
				{
					msg.id = user_index;
					msg.type = TYPE_PLAYER;
					msg.dir = DIR_LEFT_STOP;
					msg.isPushed = true;
					break;
				}
				case SC_PLAYER_RIGHT_DOWN:
				{
					msg.id = user_index;
					msg.type = TYPE_PLAYER;
					msg.dir = DIR_RIGHT_GO;
					msg.isPushed = true;
					break;
				}
				case SC_PLAYER_RIGHT_UP:
				{
					msg.id = user_index;
					msg.type = TYPE_PLAYER;
					msg.dir = DIR_RIGHT_STOP;
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

	std::queue <Message> MsgQueue;
	Message Msg;
	Col_Player_data col_player_data[3];
	Col_Camera_data col_camera_data[3];
	int retval;

	for (int i = 0; i < count_s; i++) {
		col_player_data[i].Posvec = glm::vec3(0.0f, -3.5f, 0.0f);
		col_player_data[i].PosMat = glm::translate(col_player_data[i].PosMat, col_player_data[i].Posvec);
		col_player_data[i].rad = (-120.0f * (1+i));
		col_player_data[i].RotMat = glm::rotate(col_player_data[i].RotMat, glm::radians(col_player_data[i].rad), glm::vec3(0.0f, 0.0f, 1.0f));
		col_player_data[i].SclMat = glm::scale(col_player_data[i].SclMat, glm::vec3(1.0f, 0.3f, 2.0f));
		col_player_data[i].dirVec = glm::vec3(0.0f, 0.0f, 1.0f);
		col_player_data[i].Speed = 0.0f;

		col_camera_data[i].cameraPos = glm::vec3(0.0f, 2.0f, -10.0f);
		col_camera_data[i].cameraDirection = col_camera_data[i].AT;
		col_camera_data[i].cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		col_camera_data[i].AT = glm::vec3(0, 0, 0.0f);
		col_camera_data[i].view = glm::lookAt(col_camera_data[i].cameraPos, col_camera_data[i].cameraDirection, col_camera_data[i].cameraUp);
	}

	while (true) {
		MsgQueue = glo_MsgQueue;

		//cout << "하이4" << endl;
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);
		fDeltaTime = (time.QuadPart - tTime.QuadPart) / (float)tSecond.QuadPart;
		tTime = time;

		fDeltaTime *= 100;

		for (int i = 0; i < count_s; i++) {
			if (col_player_data[i].Speed < 1.5)
			{
				col_player_data[i].Speed += acc * fDeltaTime;
			}
			col_player_data[i].Posvec.z += col_player_data[i].Speed * fDeltaTime;

			col_player_data[i].PosMat = glm::translate(col_player_data[i].PosMat, glm::vec3(0.0f, 0.0f, col_player_data[i].Speed * fDeltaTime));
		}

		while (!glo_MsgQueue.empty()) {

			glo_MsgQueue.pop();
		}

		while (!MsgQueue.empty()) {



			Msg = MsgQueue.front();
			MsgQueue.pop();
			if (Msg.type == TYPE_PLAYER) {
				switch (Msg.dir)
				{
				case DIR_LEFT_GO://왼쪽

					col_player_data[Msg.id].RotMat = glm::rotate(col_player_data[Msg.id].RotMat, glm::radians(-col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));
					col_player_data[Msg.id].Posvec = glm::rotate(col_player_data[Msg.id].Posvec, glm::radians(-col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));

					col_player_data[Msg.id].rad += 2.0f * col_player_data[Msg.id].Speed;
					if (col_player_data[Msg.id].rad > 360)
					{
						col_player_data[Msg.id].rad -= 360;
					}

					col_player_data[Msg.id].RotMat = glm::rotate(col_player_data[Msg.id].RotMat, glm::radians(col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));
					col_player_data[Msg.id].Posvec = glm::rotate(col_player_data[Msg.id].Posvec, glm::radians(col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));

					break;
				case DIR_LEFT_STOP:
					cout << "하이7" << endl;

					break;
				case DIR_RIGHT_GO://왼쪽
					col_player_data[Msg.id].RotMat = glm::rotate(col_player_data[Msg.id].RotMat, glm::radians(-col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));
					col_player_data[Msg.id].Posvec = glm::rotate(col_player_data[Msg.id].Posvec, glm::radians(-col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));

					col_player_data[Msg.id].rad -= 2.0f * col_player_data[Msg.id].Speed;
					if (col_player_data[Msg.id].rad < 0)
					{
						col_player_data[Msg.id].rad += 360;
					}

					col_player_data[Msg.id].RotMat = glm::rotate(col_player_data[Msg.id].RotMat, glm::radians(col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));
					col_player_data[Msg.id].Posvec = glm::rotate(col_player_data[Msg.id].Posvec, glm::radians(col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));

					break;
					/*case DIR_RIGHT:
						phyPlayers[phyMsg.id].SetKeyD(phyMsg.isPushed);
						break;*/
				case DIR_RIGHT_STOP:
					break;
				}
			}

			if (Msg.type == TYPE_BULLET) {
				break;
			}
		}
		//카메라 정보 업데이트 
		for (int i = 0; i < count_s; i++) {
			col_camera_data[i].posx = col_player_data[i].Posvec.x;
			col_camera_data[i].posy = col_player_data[i].Posvec.y;
			col_camera_data[i].posz = col_player_data[i].Posvec.z;
			col_camera_data[i].rotate = col_player_data[i].rad;
			col_camera_data[i].pSpeed = col_player_data[i].Speed * fDeltaTime;
			col_camera_data[i].AT = glm::vec3(0, 0, 0.0f);
		}
		//데이터 바꿔치기

		for (int i = 0; i < count_s; i++) {
<<<<<<< HEAD
			player_data[i].PosMat = col_player_data[i].PosMat;
			player_data[i].Posvec = col_player_data[i].Posvec;
			player_data[i].SclMat = col_player_data[i].SclMat;
			player_data[i].RotMat = col_player_data[i].RotMat;
			player_data[i].rad = col_player_data[i].rad;
			player_data[i].camera_posx = col_camera_data[i].posx;
			player_data[i].camera_posy = col_camera_data[i].posy;
			player_data[i].camera_posz = col_camera_data[i].posz;
			//cout << col_player_data[i].Speed << endl;
			game_data.player_data[i].PosMat = col_player_data[i].PosMat;
			game_data.player_data[i].Posvec = col_player_data[i].Posvec;
			game_data.player_data[i].SclMat = col_player_data[i].SclMat;
			game_data.player_data[i].RotMat = col_player_data[i].RotMat;
			game_data.player_data[i].camera_posx = col_camera_data[i].posx;
			game_data.player_data[i].camera_posy = col_camera_data[i].posy;
			game_data.player_data[i].camera_posz = col_camera_data[i].posz;
		

		}
		int len = sizeof(game_data);

		for (int i = 0; i < count_s; i++) { //플레이어 수만큼
			retval = send(Client_sock[i], (char*)&len, sizeof(int), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");

			}
			retval = send(Client_sock[i], (char*)&game_data, sizeof(game_data), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");

			}
		}

	}
}

void init_player(Col_Player_data cpd) {

	cpd.Posvec = glm::vec3(0.0f, -3.5f, 0.0f);
	cpd.PosMat = glm::mat4(1.0f);
	cpd.PosMat = glm::translate(cpd.PosMat, cpd.Posvec); //
	cpd.rad = 0.0f;
	cpd.RotMat = glm::mat4(1.0f);
	cpd.SclMat = glm::mat4(1.0f);//
	cpd.SclMat = glm::scale(cpd.SclMat, glm::vec3(1.0f, 0.3f, 2.0f));
	cpd.dirVec = glm::vec3(0.0f, 0.0f, 1.0f);
	cpd.Speed = 0.0f;

};

//void update_time() {
//	LARGE_INTEGER time;
//	QueryPerformanceCounter(&time);
//	fDeltaTime = (time.QuadPart - tTime.QuadPart) / (float)tSecond.QuadPart;
//	tTime = time;
//
//	fDeltaTime *= 100;
//}
//
//void update_player(Col_Player_data cpd) {
//
//	if (cpd.Speed < 1.5)
//	{
//		cpd.Speed += acc * fDeltaTime;
//	}
//	cpd.Posvec.z += cpd.Speed * fDeltaTime;
//
//	cpd.PosMat = glm::translate(cpd.PosMat, glm::vec3(0.0f, 0.0f, cpd.Speed * fDeltaTime));
//}
//
//void update_camera(Col_Player_data cpd, Col_Camera_data ccd) {
//	ccd.posx = cpd.Posvec.x;
//	ccd.posy = cpd.Posvec.y;
//	ccd.posz = cpd.Posvec.z;
//	ccd.rotate = cpd.rad;
//	ccd.pSpeed = cpd.Speed * fDeltaTime;
//	ccd.AT = glm::vec3(0, 0, 0.0f);
//}
//
//void init_camera(Col_Camera_data ccd)
//{
//	ccd.cameraPos = glm::vec3(0.0f, 2.0f, -10.0f);
//	ccd.cameraDirection = ccd.AT;//
//	ccd.cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//	ccd.AT = glm::vec3(0, 0, 0.0f);
//	ccd.view = glm::lookAt(ccd.cameraPos, ccd.cameraDirection, ccd.cameraUp);
//}