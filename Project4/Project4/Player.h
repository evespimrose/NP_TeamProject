#pragma once
#include "Camera.h"
#include "Bullet.h"
#include "framework.h"


class Player
{
private:
	bool loadOBJ(const char* path,
		std::vector<glm::vec3>& out_vertices,
		std::vector<glm::vec2>& out_uvs,
		std::vector<glm::vec3>& out_normals
	);

	Camera camera;

	glm::vec3 dirVec;

	float rad;
	float specular = 0.8f;
	float diffuse = 0.2f;
	float shininess = 256;

	// 0 = Left_keyDown, 1 = Right_keyDown, 2 = Space_keyDown
	bool keyDownlist[3];

	float posx;
	float posy;
	float posz;

	std::chrono::system_clock::time_point PrevFireTime;

	//LARGE_INTEGER tSecond;
	//LARGE_INTEGER tTime;

	float fDeltaTime;

	float Speed;

	//float acc;

	glm::vec3 PosVec;

	glm::mat4 PosMat;
	glm::mat4 RotMat;
	glm::mat4 SclMat;

	glm::mat4 TR;
	GLuint VAO;
	GLuint VBO[3];
	GLuint EBO;

	float Sphere[144][3];
	float SphereNormal[144][3];
	float SphereColor[144][3];

	GLuint BulletVAO;
	GLuint BulletVBO[3];
	GLuint BulletEBO;

	//int Life;

	int p_user_id;

	std::vector<Bullet> BulletList;

	const GLfloat Cube[8][3] = {
		{-0.2f, -0.2f, -0.2f}, // 0
		{0.2f, -0.2f, -0.2f},  // 1
		{0.2f, 0.2f, -0.2f},   // 2
		{-0.2f, 0.2f, -0.2f},  // 3

		{-0.2f, -0.2f, 0.2f},  // 4
		{0.2f, -0.2f, 0.2f},   // 5
		{0.2f, 0.2f, 0.2f},    // 6
		{-0.2f, 0.2f, 0.2f}    // 7
	};

	const GLfloat Normal[8][3] = {
		{-1, -1, -1},
		{1, -1, -1},
		{1, 1, -1},
		{-1, 1, -1},
		{-1, -1, 1},
		{1, -1, 1},
		{1, 1, 1},
		{-1, 1, 1}
	};

	const GLint Indices[12][3] = {
		{1, 0, 2},
		{2, 0, 3},
		{7, 0, 4},
		{3, 0, 7},
		{0, 1, 5},
		{4, 0, 5},
		{7, 4, 6},
		{6, 4, 5},
		{5, 1, 2},
		{5, 2, 6},
		{2, 3, 6},
		{3, 7, 6}
	};

	const GLfloat Color[8][3] = {
		{0.7, 0.7, 0.7},
		{0.7, 0.7, 0.7},
		{0.7, 0.7, 0.7},
		{0.7, 0.7, 0.7},
		{0.7, 0.7, 0.7},
		{0.7, 0.7, 0.7},
		{0.7, 0.7, 0.7},
		{0.7, 0.7, 0.7}
	};

public:
	void Init();
	void Init(Player_data pd);
	void Move(Player_data pd);
	void Update(Player_data pd);
	void Key_Input(unsigned char key, bool state);
	void sKey_Input(SOCKET sock, int key, bool state);
	void Render(GLuint ShaderProgram);
	float getSpeed();
	float getRotate();
	//bool collision();
	//bool MinusLife();
	//int getLife();
	//void setSpeed(float speed);
	void Fire();
	std::vector<Bullet> getBulletList();
	void setBulletList(std::vector<Bullet> tmpList);
	void ManageBullet();
	void setRad(float radian);
	glm::vec3 getPosition();
	Camera getCamera();
	bool* getKey() { return keyDownlist; }

	void Reset();
};