#include "framework.h"
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

SOCKET init_sock()
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	//connect
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(Proto_IP);
	serveraddr.sin_port = htons(Proto_Port);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	return sock;
}

void send_Player(SOCKET sock, Player_data player)
{
	int retval;

	int len = sizeof(player);
	retval = send(sock, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		exit(1);
	}
	retval = send(sock, (char*)&player, sizeof(Player_data), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		exit(1);
	}
}
void Send_event(SOCKET sock, char buf)
{
	send(sock, &buf, sizeof(char), 0);
}

int recvn(SOCKET s, char* buf, int len, int flags) {
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
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

Player_data recv_Player(SOCKET sock) {
	int retval;
	int len;
	std::vector<Cube_data> CubeVec;

	retval = recvn(sock, (char*)&len, sizeof(int), 0); // 데이터 받기(고정 길이)
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}
	else if (retval == 0) {
	}

	//printf( "Recv : %d\n", len );
	//printf( "데이터 크기 : %d\n", len );

	int GetSize;
	char suBuffer[BUFSIZE];
	Player_data* player;
	GetSize = recv(sock, suBuffer, len, 0);
	if (GetSize == SOCKET_ERROR) {
		MessageBox(NULL, "서버와의 연결이 끊어졌습니다..!\n이유 : \n 1. 서버가 종료되었습니다\n 2. 서버에 의해 강제퇴장 당하였습니다\n 3. 인터넷 연결이 올바르지 않습니다", "NetworkTermProject", 0);
		exit(1);
	}

	suBuffer[GetSize] = '\0';
	player = (Player_data*)suBuffer;

	//printf( "%d\n", player[0]->camxrotate );

	///////////////////////////////////////////////////


	Cube_data* Cube_data_Array;

	retval = recvn(sock, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}

	
	retval = recvn(sock, (char*)&suBuffer, len, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}

	Cube_data_Array = (Cube_data*)suBuffer;

	//for (int i = 0; i < len; ++i)
	//	printf("%f\n", Cube_data_Array[i].zOffset);


	return *player;
}

