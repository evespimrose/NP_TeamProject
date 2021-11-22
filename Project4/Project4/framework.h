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

typedef struct Game_Communication_Data
{
    BOOL Im_Ready = false;
    BOOL Players_Pt[3];
    BOOL Players_Ready[3] ; 
    BOOL IS_START = false; //게임 시작했는지
}Game_Communication_Data;

void err_quit(const char* msg);
void err_display(const char* msg);
SOCKET init_sock();
void send_Player(SOCKET sock, Player_data player);