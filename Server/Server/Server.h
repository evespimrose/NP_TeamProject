#pragma once
#include <map>
#include <queue>

struct Message {
    char id;
    char type;
    char dir;
    bool isPushed;
};

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

typedef struct login_info
{
    BOOL is_connected;
    short size;
    char type;
    char id;
}login_info;

typedef struct ready_info
{
    int id = -1;
    short size = 0;
    BOOL pt_player{ false };
    BOOL is_ready{ false };
}ready_info;

typedef struct all_ready_info
{
    BOOL Pt_Players[3]{ false };
    BOOL is_ready[3]{ false };
    BOOL game_start{ false };
}all_ready_info;

typedef struct ConnectClient {
    bool is_connected;
}ConnectClient;
ConnectClient connectedCls[3]{ false };
std::map <char, SOCKET> clients;
all_ready_info ari;
ready_info* ri;
SOCKET Client_sock[3];

std::queue <Message> glo_MsgQueue;