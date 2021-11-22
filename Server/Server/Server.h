#pragma once
#define MAX_CLIENT 3

// structs for send
typedef struct Player_data
{
    float PosVec_z = -1000.0f;
    float rotate = -1000.0f;
    float speed = -1000.0f;
    int ID = -1000;
    //vector<Bullet> BulletList;
    bool* KeyDownlist;
}Player_data;

typedef struct Data
{
    Player_data PlayerData;
}Data;

typedef struct Server_data
{
    Data dat[MAX_CLIENT];
}Server_data;


typedef struct Game_Communication_Data
{
    BOOL Im_Ready = false;
    BOOL Players_Pt[3];
    BOOL Players_Ready[3];
    BOOL IS_START = false; //게임 시작했는지
}Game_Communication_Data;

