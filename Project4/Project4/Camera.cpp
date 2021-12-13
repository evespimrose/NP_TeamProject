#include "Camera.h"

void Camera::setpSpeed(float s)
{
	pSpeed = s;
}

void Camera::setRotate(float r)
{
	rotate = r;
}

void Camera::setPosition(float p_z)
{
	posz = p_z;

}
glm::vec3 Camera::getPosition()
{
	return glm::vec3(posx, posy - 1.0f, posz - 30.0f);
}

void Camera::setAT()
{
	AT = glm::vec3(0, 0, 0.0f);
}

void Camera::rotatecamera(float rad)
{
	view = glm::translate(view, glm::vec3(0.0f, -1.0f, 0.0f));
	view = glm::rotate(view, glm::radians(-rad), glm::vec3(0.0f, 0.0f, 1.0f));
	view = glm::translate(view, glm::vec3(-posx, -posy, -posz));
}

void Camera::init()
{
	glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, -10.0f);
	glm::vec3 cameraDirection = AT;
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
}

void Camera::Render(GLuint ShaderProgram)
{
	init();

	glm::mat4 Projection = glm::mat4(1.0f);
	Projection = glm::perspective(glm::radians(45.0f), (float)1000 / (float)1000, 0.1f, 1000.0f);
	unsigned int ProjectionLocation = glGetUniformLocation(ShaderProgram, "projectionTransform");;
	glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, &Projection[0][0]);

	rotatecamera(rotate);

	unsigned int viewLocation = glGetUniformLocation(ShaderProgram, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
}

void Camera::setcamera_pos(float x, float y, float z) {
	posx = x;
	posy = y;
	posz = z;
}