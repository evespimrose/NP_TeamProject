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
//typedef struct Data
//{
//    Player_data PlayerData;
//}Data;

typedef struct Cube
{
    float zOffset;
    float rotate;
    int life;
};

typedef struct Col_Player_data //계산용 플레이어 데이터
{
    glm::vec3 Posvec = glm::vec3(0.0f, -3.5f, 0.0f);
    glm::mat4 PosMat = glm::mat4(1.0f);
    glm::mat4 SclMat = glm::mat4(1.0f);
    glm::vec3 dirVec = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::mat4 RotMat = glm::mat4(1.0f);
    float rad = 0.0f;
  //  float posx;
  //  float posy;
  //  float posz;
    float Speed = 0.0f;
}Col_Player_data;

typedef struct Col_Camera_data //계산용 플레이어 데이터
{
    float posx = 0.0f;
    float posy = 0.0f;
    float posz = 0.0f;
    float pSpeed = 0.0f;
    float rotate = 0.0f;
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 AT = glm::vec3(0, 0, 0.0f);
   // glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, -10.0f);
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 1.0f);

}Col_Camera_data;

typedef struct Player_data//최종 보낼 플레이어 데이터
{
    glm::vec3 Posvec = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 PosMat= glm::mat4(1.0f);
    glm::mat4 SclMat= glm::mat4(1.0f);
    glm::mat4 RotMat= glm::mat4(1.0f);
    float rad = 0.0f;
    float speed = 0.0f;
    float camera_posx = 0.0f;
    float camera_posy = 0.0f;
    float camera_posz = 0.0f;
    //vector<Bullet> BulletList;
}Player_data;

typedef struct Game_data//최종 보낼 플레이어 데이터
{
    Player_data player_data[3];
}Game_data;


LARGE_INTEGER tSecond;
LARGE_INTEGER tTime;
float acc= 0.0005f;
float fDeltaTime=0;

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
    BOOL im_game_start{ false };
}ready_info;

typedef struct all_ready_info
{
    BOOL Pt_Players[3]{ false };
    BOOL is_ready[3]{ false };
    int pt_clients_num;
    BOOL game_start{ false };
}all_ready_info;

typedef struct ConnectClient {
    bool is_connected;
}ConnectClient;
ConnectClient connectedCls[3]{ false };
std::map <char, SOCKET> clients;
all_ready_info ari;
ready_info* ri; //보냄
SOCKET Client_sock[3];

std::queue <Message> glo_MsgQueue;

bool game_start = false;