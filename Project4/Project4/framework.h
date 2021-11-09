#pragma once
#include <fstream>
#include <iostream>
#include <string>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <time.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <math.h>

#include <stdio.h>
#include <time.h>
#include <chrono>

// Ŭ���̾�Ʈ ��� ���� ���� ���
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>

#define Proto_IP "127.0.0.1"
#define Proto_Port 9000

struct Player_data
{
    glm::vec3 PosVec;
    float speed;
    //string ID;
    //vector<Bullet> BulletList;
    bool KeyDownlist[3];
};

struct Cube_data
{
    glm::vec3 PosVec;
    int life;
};

struct Data
{
    Player_data PlayerData;
    Cube_data CubeData;
};

void Clientsock()
{
    int retval;

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(Proto_IP);
    serveraddr.sin_port = htons(Proto_Port);

    retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
}