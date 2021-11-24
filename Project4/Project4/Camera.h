#pragma once
#include "framework.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
	float posx;
	float posy;
	float posz;

	float pSpeed;

	float rotate;

	glm::vec3 cameraUp;
	glm::vec3 AT;
	glm::mat4 view = glm::mat4(1.0f);

public:
	void Render(GLuint ShaderProgram);
	void setpSpeed(float s);
	void setRotate(float r);
	void setPosition(glm::vec3 p);
	void setAT();
	glm::vec3 getPosition();
	void rotatecamera(float rad);
	void init();
	void Roll(float fRadian);
};