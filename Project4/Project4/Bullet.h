#pragma once
#include "framework.h"
#include "sound.h"

class Bullet
{
private:
	float specular = 0.8f;
	float diffuse = 0.2f;
	float shininess = 256;

	float rotate;
	GLuint VAO;
	glm::mat4 PosMat;
	glm::mat4 RotMat;
	float Speed;
	glm::vec3 PosVec;

	LARGE_INTEGER tSecond;
	LARGE_INTEGER tTime;

	float fDeltaTime;

public:
	void Bullet::Init(glm::vec3 playerPos, GLuint vao, float PlayerSpeed, float rad);
	void Bullet::Init(Player_data pd, GLuint vao);
	void Render(GLuint ShaderProgram);
	void Move();
	float getzOffset();
	float getRotate();
};