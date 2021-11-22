#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "Server.h"

using namespace std;

#define SERVERPORT 9000
#define BUFSIZE 1024
#define MAXPLAYER 3


DWORD WINAPI recv_thread(LPVOID arg);
//소켓함수 오류 출력 후 종료
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
	ready_info rf_Packet;
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
		//클아이언트에게 아이디 부여 
		/*for (int i = 0; i < MAXPLAYER; ++i) {
			if (false == connectedCls[i].is_connected) {
				connectedCls[i].is_connected = true;
				user_index = i;
				clients_count++;
				cout << "==============================" << endl;
				cout << "ID " << i << " is connected" << endl;
				cout << "==============================" << endl;

				break;
			}
		}

		*/
		Client_sock[count_s] = client_sock;
		count_s++;
		//

		////접속한 클라에게 id 전송
		//login_info li_packet;
		//li_packet.id = user_index;
		//li_packet.size = sizeof(li_packet);
		//li_packet.type = true;
		//send(client_sock, (char*)&li_packet, sizeof(li_packet), 0);

		////레디 상황 전달
		//for (int i = 0; i < MAXPLAYER; ++i)	// 유저아이디가 순서대로 증가할 때만 가능
		//{
		//	if (true == connectedCls[i].is_connected)
		//	{
		//		send(clients[i], (char*)&rf_Packet, sizeof(rf_Packet), 0);
		//	}
		//}

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
	//--------------------------------------------------------------------------------

	int GetSize;
	char Buffer[BUFSIZE];
	ready_info* ri;

	//BOOL is_ready[3]{ false }; //비준비 상태로 초기화

	while (1) { //일단 무한반복
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
		cout << ari.is_ready[0] << endl;
		cout << ari.is_ready[1] << endl;
		cout << ari.is_ready[2] << endl;
	}
	//---------------------------------------------------------------------------------------------

	return 0;
}

