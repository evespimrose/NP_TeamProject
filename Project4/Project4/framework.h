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

// 클라이언트 통신 관련 참조 헤더
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "glew32")
#include <winsock2.h>

#define Proto_IP "127.0.0.1"
#define Proto_Port 9000
#define BUFSIZE    1024

// structs for send
typedef struct Player_data
{
    float PosVec_z;
    float rotate;
    float speed;
    int ID;
    //vector<Bullet> BulletList;
    bool* KeyDownlist;
}Player_data;

extern Player_data player_data;

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

typedef struct ready_info
{
    int id = -1;
    short size = 0;
    BOOL pt_player{ false };
    BOOL is_ready{ false };
}ready_info;



typedef struct all_ready_info //받는 유저들의 데이터
{
    BOOL Pt_Players[3]{ false };
    BOOL is_ready[3]{ false };
    BOOL game_start{ false };
}all_ready_info;

void err_quit(const char* msg);
void err_display(const char* msg);
SOCKET init_sock();
void send_Player(SOCKET sock, Player_data player);
Player_data recv_Player(SOCKET sock);
void Send_event(SOCKET sock, char buf);