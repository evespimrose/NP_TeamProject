#pragma once
#include <queue>
#include <vector>
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
using namespace std;

typedef struct Col_Player_data //계산용 플레이어 데이터
{
    glm::vec3 Posvec = glm::vec3(0.0f, -3.5f, 0.0f);
    glm::mat4 PosMat = glm::mat4(1.0f);
    glm::mat4 SclMat = glm::mat4(1.0f);
    glm::vec3 dirVec = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::mat4 RotMat = glm::mat4(1.0f);
    float rad = 0.0f;

    float Speed = 0.0f;
    glm::mat4 TR = RotMat * PosMat * SclMat;

}Col_Player_data;

typedef struct Col_Bullet_data//계산용 총알 데이터
{
    float rotate = 0.0f;
    glm::mat4 PosMat = glm::mat4(1.0f);
    float Speed = 0.0f;
    glm::vec3 PosVec = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 RotMat = glm::mat4(1.0f);

}Col_Bullet_data;

typedef struct Col_Cube_data {
    int life;
    float PosZ;
    glm::mat4 PosMat = glm::mat4(1.0f);
    glm::mat4 RotMat = glm::mat4(1.0f);
};

typedef struct Player_data//최종 보낼 플레이어 데이터
{
    //std::vector<int> BulletList;
    glm::vec3 Posvec = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 PosMat= glm::mat4(1.0f);
    glm::mat4 SclMat= glm::mat4(1.0f);
    glm::mat4 RotMat= glm::mat4(1.0f);
   // glm::mat4 TR = glm::mat4(1.0f);
    float rad = 0.0f;
    
}Player_data;



typedef struct Bullet_data//최종 보낼 플레이어 데이터
{
    std::vector<int> BulletList;
    glm::vec3 Posvec = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 PosMat = glm::mat4(1.0f);
    glm::mat4 SclMat = glm::mat4(1.0f);
    glm::mat4 RotMat = glm::mat4(1.0f);
    float rad = 0.0f;
    float camera_posx = 0.0f;
    float camera_posy = 0.0f;
    float camera_posz = 0.0f;


}Bullet_data;

typedef struct Game_data//최종 보낼 플레이어 데이터
{
    Player_data player_data[3];
  //  Bullet_data bullet_data[100];
}Game_data;


LARGE_INTEGER tSecond;
LARGE_INTEGER tTime;
float acc= 0.0001f;
float fDeltaTime=0;


typedef struct ready_info
{
    int id = -1;
    short size = 0;
    char pt_player;
    char is_ready;
    BOOL im_game_start{ false };
}ready_info;

bool is_connected[3] = { false, };

//ready_info* ri; //보냄

ready_info ri[3];

SOCKET Client_sock[3];

std::queue <Message> glo_MsgQueue;

int count_s = -1;
int Bullet_num = 0;//발사된 총알 개수
