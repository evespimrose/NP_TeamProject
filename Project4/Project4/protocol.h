#pragma once
#define MAX_BULLET 10
#define MAX_CUBE 10

#define CS_PLAYER_LEFT_UP 17
#define CS_PLAYER_LEFT_DOWN 18
#define CS_PLAYER_RIGHT_UP 19
#define CS_PLAYER_RIGHT_DOWN 20
///////////////////////////////////
#define SC_LOGIN_OK			1
#define SC_READY            2
#define SC_GAMESTART		3
#define SC_PLAYER_POS       4
#define SC_BULLET_POS       5
#define SC_MAP_CUBE			6
///////////////////////////////
#define CS_READY           1
#define CS_FIRE            2
struct Player_pos {
	char id;
	float PosX, PosY, PosZ;
	glm::mat4 PosMat = glm::mat4(1.0f);
	glm::mat4 SclMat = glm::mat4(1.0f);
	glm::mat4 RotMat = glm::mat4(1.0f);
	//glm::mat4 TR;
};

struct Bullet_pos {
	glm::mat4 PosMat = glm::mat4(1.0f);
};

struct Cube_pos {
	int life;
	float rad;
	glm::mat4 PosMat = glm::mat4(1.0f);
	glm::mat4 RotMat = glm::mat4(1.0f);
};

struct sc_packet_login_ok {
	short size;
	char type;
	char id;
};

struct sc_packet_ready {
	short size;
	char type;
	char id;
	char ready;
	char pt;
};


struct sc_packet_game_start {
	short size;
	char type;
	char clients_num;

};

struct sc_packet_player_pos {
	short size;
	char type;
	Player_pos players[3];
};

struct sc_packet_bullet_pos {
	short size;
	char type;
	Bullet_pos bullets[MAX_BULLET];
};

struct sc_packet_cube_pos {
	short size;
	char type;
	Cube_pos cubes[MAX_CUBE];
};