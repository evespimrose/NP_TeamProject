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
#include "packet.h"

#include <glm/gtx/string_cast.hpp>
#define SERVERPORT 9000
#define BUFSIZE 512
#define MAXPLAYER 3
#define MAX_BULLET 10

int GetSize;
char Buffer[BUFSIZE];

DWORD WINAPI recv_thread(LPVOID iD);
//DWORD WINAPI Calcutlaion_Thread(LPVOID arg);
void Calcutlaion_clients();
void init_player(Col_Player_data cpd);
//void update_player(Col_Player_data cpd);
//void update_time();

 CRITICAL_SECTION cs;
 CRITICAL_SECTION Msg_cs;
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



bool Is_GameStart() {
	int count = 0;

	for (int i = 0; i < count_s+1; ++i)
	{
		if (is_connected != false)
		{
			if (ri[i].is_ready == true)
			{
				count += 1;
			}
		}
	}

	std::cout << "numof cl : " << count_s + 1 << ", count : " << count << std::endl;

	if (count_s + 1 > 1 && count == count_s + 1)
		return true;
	else
		return false;

}

int main()
{
	//임계 영역 생성
	InitializeCriticalSection(&cs);
	InitializeCriticalSection(&Msg_cs);
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

	int clients_count = 0;
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

		for (int i = 0; i < MAXPLAYER; ++i) {
			if (false == is_connected[i]) {
				is_connected[i] = true;
				user_index = i;
				count_s++;
				cout << "==============================" << endl;
				cout << "ID " << i << " is connected" << endl;
				cout << "==============================" << endl;

				break;
			}
		}

		Client_sock[count_s] = client_sock;
		
		
		ri[user_index].id = user_index;
		ri[user_index].pt_player = 1;
		ri[user_index].is_ready = 0;

		SendLoginOkPacket(user_index);
		//나에게 나머지의 레디상황
	//나머지 클라에게 나의 레디상황
		for (int i = 0; i < MAXPLAYER; ++i)	// 유저아이디가 순서대로 증가할 때만 가능
		{
			if (true == is_connected[i])
			{
				SendReadyPacket(Client_sock[i], user_index);	//다른 클라에게 내 ready상태 전송
			                                                                
				SendReadyPacket(Client_sock[user_index], i);	//내 클라에게 다른 클라 ready상태 전송
			
			}
			
		}

		//recv 스레드 생성
		hThread = CreateThread(NULL, 0, recv_thread, (LPVOID)user_index, 0, NULL);
		if (hThread == NULL) {
			closesocket(client_sock);
		}
		else {
			CloseHandle(hThread);
		}


	}

	DeleteCriticalSection(&cs);
	DeleteCriticalSection(&Msg_cs);
	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;

}

DWORD WINAPI recv_thread(LPVOID iD) {

	char id = (char)iD;

	SOCKET client_sock = Client_sock[id];
	//LeaveCriticalSection(&cs);
	Message msg{};

	while (true)
	{

		msg.id = -1;
		char buf[BUFSIZE];
		//cout << "여기1" << endl;
		char retval = recv(client_sock, buf, 1, 0);
		cout << retval << endl;
		// 클라이언트 접속 종료, recv 에러 처리
		if (retval == 0 || retval == SOCKET_ERROR) {
			closesocket(client_sock);
			cout << "여기55" << endl;
			/*	g_clients.erase(id);

				for (auto& cl : g_clients)
					SendRemovePlayerPacket(cl.first, id);

				g_connectedCls[id].is_connected = false;
				g_connectedClsLock.unlock();
				--numOfCls;*/

				//cout << "======================================================" << endl;
				//cout << "ID " << (int)id << " is out. And this Id slot is empty" << endl;
				//cout << "======================================================" << endl;

			/*	if (retval == SOCKET_ERROR)
					ErrDisplay("RecvThread occured Error!");*/
			return 0;
		}
		// Message 재사용을 위한 초기화
		ZeroMemory(&msg, sizeof(Message));
		msg.id = -1;
		switch ((int)buf[0])
		{
		case CS_PLAYER_LEFT_DOWN:
		{
			//cout << "왼쪽" << endl;
			msg.id = id;
			msg.type = TYPE_PLAYER;
			msg.dir = DIR_LEFT_GO;
			msg.isPushed = true;
			break;
		}
		case CS_PLAYER_RIGHT_DOWN:
		{
			//cout << "오른쪽" << endl;
			msg.id = id;
			msg.type = TYPE_PLAYER;
			msg.dir = DIR_RIGHT_GO;
			msg.isPushed = true;
			break;
		}

		case CS_READY:
		{
			if (ri[id].is_ready) {
				ri[id].is_ready = 0;
			}
			else {
				ri[id].is_ready = 1;
			} //ready상태 갱신

			for (auto& Csock : Client_sock) //다른 클라한테 ri정보 send
				SendReadyPacket(Csock, id);

			if (Is_GameStart())
			{
				SendGameStartPacket();
				cout << "게임시작" << endl;
				// Physics Thread 생성
				std::cout << "physics thread 생성!" << std::endl;
				thread PhysicsThread(Calcutlaion_clients);
				PhysicsThread.detach();
			}
			break;
		}
		case CS_FIRE:
			msg.id = id;
			msg.type = TYPE_BULLET;
			msg.dir = SHOOT_FIRE;
			msg.isPushed = true;

			break;
		default:
			cout << "Packet Type Error! - " << buf[0] << endl;
			while (true);
		}
		if (msg.id != -1)
		{
			glo_MsgQueue.emplace(msg);
		}
	}
}

void Calcutlaion_clients() {
	std::cout << "physics thread 생성!" << std::endl;
	QueryPerformanceFrequency(&tSecond);
	QueryPerformanceCounter(&tTime);


	std::queue <Message> MsgQueue;
	Message Msg;
	Col_Player_data col_player_data[3];
	Col_Bullet_data cbd[MAX_BULLET];
	Bullet_pos bullets[MAX_BULLET];
	Col_Cube_data ccd[MAX_CUBE];
	Cube_pos cubes[MAX_CUBE];
	glm::mat4 fixed_RotMat[3];

	int retval;

	for (int i = 0; i <3; i++) {
		col_player_data[i].Posvec = glm::vec3(0.0f, -3.5f, 0.0f);
		col_player_data[i].PosMat = glm::translate(col_player_data[i].PosMat, col_player_data[i].Posvec);
		col_player_data[i].rad = (120.0f * (1+i));
		col_player_data[i].RotMat = glm::rotate(col_player_data[i].RotMat, glm::radians(-col_player_data[i].rad), glm::vec3(0.0f, 0.0f, 1.0f));
		fixed_RotMat[i] = col_player_data[i].RotMat;
		col_player_data[i].SclMat = glm::scale(col_player_data[i].SclMat, glm::vec3(1.0f, 0.3f, 2.0f));
		col_player_data[i].dirVec = glm::vec3(0.0f, 0.0f, 1.0f);
		col_player_data[i].Speed = 0.0f;
		col_player_data[i].TR = col_player_data[i].RotMat * col_player_data[i].PosMat * col_player_data[i].SclMat;
	}

	Player_pos players[3];
	for (int i = 0; i < 3; i++) {
		players[i].PosX = 0.0f;
		players[i].PosY = 0.0f;
		players[i].PosZ = 0.0f;
		//players [i].TR=col_player_data[i].TR;
		players[i].PosMat = glm::translate(col_player_data[i].PosMat, col_player_data[i].Posvec);
		players[i].RotMat = glm::rotate(col_player_data[i].RotMat, glm::radians(col_player_data[i].rad), glm::vec3(0.0f, 0.0f, 1.0f));
		players[i].SclMat = col_player_data[i].SclMat ;
	}

	for (int i = 0; i < MAX_CUBE; ++i)
	{
		Col_Cube_data c;
		c.life = rand() % 3;
		c.PosZ = 0.0f;
		glm::vec3 cpos = glm::vec3(0.0f, -3.5f, c.PosZ);
		c.rad = rand() % 360;
		c.PosMat = glm::translate(c.PosMat, cpos);
		c.RotMat = glm::rotate(c.RotMat, glm::radians(c.rad), glm::vec3(0.0f, 0.0f, 1.0f));
		ccd[i] = c;
	}

	while (true) {
		EnterCriticalSection(&Msg_cs);
		MsgQueue = glo_MsgQueue;
		LeaveCriticalSection(&Msg_cs);

		//cout << "하이4" << endl;
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);
		fDeltaTime = (time.QuadPart - tTime.QuadPart) / (float)tSecond.QuadPart;
		tTime = time;

		fDeltaTime *= 100;

		for (int i = 0; i < count_s+1; i++) {
			if (col_player_data[i].Speed < 1.5)
			{
				col_player_data[i].Speed += acc * fDeltaTime;
			}
			col_player_data[i].Posvec.z += col_player_data[i].Speed * fDeltaTime;

			col_player_data[i].PosMat = glm::translate(col_player_data[i].PosMat, glm::vec3(0.0f, 0.0f, col_player_data[i].Speed * fDeltaTime));
		}

		EnterCriticalSection(&Msg_cs);
		while (!glo_MsgQueue.empty()) {
			glo_MsgQueue.pop();
		LeaveCriticalSection(&Msg_cs);
		}

		while (!MsgQueue.empty()) {
			//cout << "뭔가 왔음" << endl;


			Msg = MsgQueue.front();
			MsgQueue.pop();
			if (Msg.type == TYPE_PLAYER) {
				switch (Msg.dir)
				{
				case DIR_LEFT_GO://왼쪽
					//cout << "왼쪽 누름" << endl;
					col_player_data[Msg.id].RotMat = glm::rotate(col_player_data[Msg.id].RotMat, glm::radians(-col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));
					col_player_data[Msg.id].Posvec = glm::rotate(col_player_data[Msg.id].Posvec, glm::radians(-col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));

					col_player_data[Msg.id].rad += 30.0f * col_player_data[Msg.id].Speed;
					if (col_player_data[Msg.id].rad > 360)
					{
						col_player_data[Msg.id].rad -= 360;
					}

					col_player_data[Msg.id].RotMat = glm::rotate(col_player_data[Msg.id].RotMat, glm::radians(col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));
					col_player_data[Msg.id].Posvec = glm::rotate(col_player_data[Msg.id].Posvec, glm::radians(col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));

					break;
		
				case DIR_RIGHT_GO://오른쪽
					//cout << "오른쪽 누름" << endl;
					col_player_data[Msg.id].RotMat = glm::rotate(col_player_data[Msg.id].RotMat, glm::radians(-col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));
					col_player_data[Msg.id].Posvec = glm::rotate(col_player_data[Msg.id].Posvec, glm::radians(-col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));

					col_player_data[Msg.id].rad -= 30.0f * col_player_data[Msg.id].Speed;
					if (col_player_data[Msg.id].rad < 0)
					{
						col_player_data[Msg.id].rad += 360;
					}

					col_player_data[Msg.id].RotMat = glm::rotate(col_player_data[Msg.id].RotMat, glm::radians(col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));
					col_player_data[Msg.id].Posvec = glm::rotate(col_player_data[Msg.id].Posvec, glm::radians(col_player_data[Msg.id].rad), glm::vec3(0.0f, 0.0f, 1.0f));

					break;
				
				}
			}

			if (Msg.type == TYPE_BULLET) {
				cout << col_player_data[Msg.id].Posvec.x << endl;
				
				cbd[Bullet_num].PosVec = col_player_data[Msg.id].Posvec;
				cbd[Bullet_num].PosVec.z += 0.5f;
				cbd[Bullet_num].PosMat = fixed_RotMat[Msg.id] * cbd[Bullet_num].PosMat;
				cbd[Bullet_num].PosMat = glm::translate(cbd[Bullet_num].PosMat, cbd[Bullet_num].PosVec);

				cbd[Bullet_num].rotate = col_player_data[Msg.id].rad;
				cbd[Bullet_num].Speed = col_player_data[Msg.id].Speed + 0.3f;
				
				Bullet_num++;
				break;
			}
		}
		//총알 정보 업데이트 
		for (int i = 0; i < Bullet_num; i++) {
			cbd[i].PosVec.z += cbd[i].Speed * fDeltaTime;
			cbd[i].PosMat = glm::translate(cbd[i].PosMat, glm::vec3(0, 0, cbd[i].Speed * fDeltaTime));
		}


		float fpz = 0.0f;
		float spz = FLT_MAX;

		//데이터 바꿔치기
		for (int i = 0; i < count_s+1; i++) {
			
			players[i].PosX = col_player_data[i].Posvec.x;
			players[i].PosY = col_player_data[i].Posvec.y;
			players[i].PosZ = col_player_data[i].Posvec.z;
			players[i].PosMat = col_player_data[i].PosMat;
			players[i].RotMat = col_player_data[i].RotMat;
			players[i].SclMat = col_player_data[i].SclMat;
			fpz = max(fpz, col_player_data[i].Posvec.z);
			spz = min(spz, col_player_data[i].Posvec.z);
		}

		for (int i = 0; i < Bullet_num; i++) { //총알 수만큼

			bullets[i].PosMat = cbd[i].PosMat;
		}
		// 큐브 갱신
		for (int i = 0; i < MAX_CUBE; ++i)
		{
			if (ccd[i].PosZ + 50.0f < spz)
			{
				Col_Cube_data c;
				c.life = rand() % 3;
				c.PosZ = fpz + 100.0f + rand() % 100;
				glm::vec3 cpos = glm::vec3(0.0f, -3.5f, c.PosZ);
				c.rad = rand() % 360;
				c.PosMat = glm::translate(c.PosMat, cpos);
				c.RotMat = glm::rotate(c.RotMat, glm::radians(c.rad), glm::vec3(0.0f, 0.0f, 1.0f));
				ccd[i] = c;
			}
		}

		for (int i = 0; i < MAX_CUBE; ++i)
		{
			cubes[i].PosMat = ccd[i].PosMat;
			cubes[i].RotMat = ccd[i].RotMat;
			cubes[i].life = ccd[i].life;
		}
		//cout << glm::to_string(ccd[0].PosMat) << std::endl;
		SendPlayerPosPacket(*players);
		SendBulletPosPacket(*bullets);
		SendCubePosPacket(*cubes);

		for (int i = 0; i < count_s + 1; ++i) {
			for (int j = 0; j < MAX_CUBE; ++j)
			{
				if (col_player_data[i].Posvec.z + 0.5f > ccd[j].PosZ &&
					col_player_data[i].Posvec.z - 0.5f < ccd[j].PosZ &&
					col_player_data[i].rad + 10 > ccd[j].rad &&
					col_player_data[i].rad - 10 < ccd[j].rad
					)
				{
					// 충돌처리 추가
				}
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
