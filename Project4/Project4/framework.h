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

#include "protocol.h"

// 클라이언트 통신 관련 참조 헤더
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "glew32")
#include <winsock2.h>

#define Proto_IP "127.0.0.1"
//#define Proto_IP "192.168.181.246"
#define Proto_Port 9000
#define BUFSIZE 1024

enum
{
    TITLE_SCENE,
    LOBBY_SCENE,
    GAME_SCENE,
    OVER_SCENE
};

// structs for send
typedef struct Player_data
{
    glm::vec3 Posvec;
    glm::mat4 PosMat;
    glm::mat4 RotMat;
    glm::mat4 SclMat;

}Player_data;

typedef struct Game_data//최종 보낼 플레이어 데이터
{
    Player_data player_data[3];
   // Bullet_data  bullet_data[100];
}Game_data;

typedef struct Cube_data
{
    glm::vec3 PosVec;
    int life;
}Cube_data;

typedef struct Data
{
    Player_data PlayerData;
    Cube_data CubeData;
}Data;

typedef struct all_ready_info
{
    char Pt_Players[3];
    char is_ready[3];
    int pt_clients_num;
    BOOL game_start{ false };
}all_ready_info;

typedef struct ready_info
{
    int id = -1;
    short size = 0;
    BOOL pt_player{ false };
    BOOL is_ready{ false };
    BOOL im_game_start{ false };
}ready_info;


//int recvn(SOCKET s, char* buf, int len, int flags);

void err_quit(const char* msg);
void err_display(const char* msg);
SOCKET init_sock();
void Recv_Packet(SOCKET sock);
void ProcessData(char* buf, size_t io_byte);
void ProcessPacket(char* packet_buffer);
void send_Player(SOCKET sock, Player_data player);
int get_ClientID(SOCKET sock);
void Send_event(SOCKET sock, char buf);
Game_data recv_Player(SOCKET sock);

