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
#include <winsock2.h>

#define Proto_IP "127.0.0.1"
#define Proto_Port 9000

// structs for send
struct Player_data
{
    glm::vec3 PosVec;
    float speed;
    int ID;
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

