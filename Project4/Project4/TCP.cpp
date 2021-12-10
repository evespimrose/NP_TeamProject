#include "framework.h"
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

SOCKET init_sock()
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	ULONG on = 1;
	BOOL optval = TRUE;

	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	//ioctlsocket(sock, FIONBIO, &on);

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

void Recv_Packet(SOCKET sock)
{
	int retval;
	char buf[BUFSIZE];
	retval = recv(sock, buf, BUFSIZE, 0);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return;
		}
		//ErrorDisplay( "recv()!" );
		return;
	}
	else if (retval == 0) {
		std::cout << "SERVER FULL! CAN'T CONNECT!" << std::endl;
		PostQuitMessage(0);
		return;
	}
	else {
		ProcessData(buf, retval);
	}
}

void ProcessData(char* buf, size_t io_byte )
{
	char* ptr = buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUFSIZE];

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = MAKEWORD(ptr[0], ptr[1]); // ó������ �޽��� ũ�⸦ ����
		if (io_byte + saved_packet_size >= in_packet_size) {   // ó������ �޽��� ũ�⺸�� ���� ���� ����Ʈ �� + �����ϰ� �ִ� ����Ʈ ���� ũ�� processPacket ȣ��, �Ѱ��� ����Ʈ ��ŭ ���� ������, ���� ����Ʈ �� ����
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);   // ���� �� ��ŭ�� �ӽ� ���� ���ۿ� ��Ƶα�.
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
	return;
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

Game_data recv_Player(SOCKET sock) {
	int retval;
	int len;
	retval = recvn(sock, (char*)&len, sizeof(int), 0); // ������ �ޱ�(���� ����)
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}
	else if (retval == 0) {
	}

	//printf( "Recv : %d\n", len );
	//printf( "������ ũ�� : %d\n", len );

	int GetSize;
	char suBuffer[BUFSIZE];
	Game_data* game_data;
	GetSize = recv(sock, suBuffer, len, 0);
	if (GetSize == SOCKET_ERROR) {
		MessageBox(NULL, "�������� ������ ���������ϴ�..!\n���� : \n 1. ������ ����Ǿ����ϴ�\n 2. ������ ���� �������� ���Ͽ����ϴ�\n 3. ���ͳ� ������ �ùٸ��� �ʽ��ϴ�", "NetworkTermProject", 0);
		exit(1);
	}

	suBuffer[GetSize] = '\0';
	game_data = (Game_data*)suBuffer;

	//printf( "%d\n", player[0]->camxrotate );

	return *game_data;
}

int get_ClientID(SOCKET sock) {
	int retval;
	int len;
	retval = recvn(sock, (char*)&len, sizeof(int), 0); // ������ �ޱ�(���� ����)
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}
	else if (retval == 0) {
	}

	char* buf = new char[len]; // ���۵� ���̸� �˰� ������ ũ�⿡ ���缭 buf�� �÷�����!

	retval = recvn(sock, buf, len, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}
	return atoi(buf);
}