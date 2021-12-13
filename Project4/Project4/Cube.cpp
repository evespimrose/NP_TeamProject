#include "Cube.h"


void Cube::Init(float Offset, GLuint* vao)
{
	for (int i = 0; i < 3; ++i)
	{
		VAO[i] = vao[i];
	}
}

void Cube::Render(GLuint ShaderProgram)
{
	unsigned int modelLocation = glGetUniformLocation(ShaderProgram, "modelTransform");
	glm::mat4 TR;
	TR = RotMat * PosMat;

	unsigned int specularLocation = glGetUniformLocation(ShaderProgram, "spec_strength");
	unsigned int diffuseLocation = glGetUniformLocation(ShaderProgram, "diffuse_strength");
	unsigned int shininessLocation = glGetUniformLocation(ShaderProgram, "shininess");

	glUniform1f(specularLocation, specular);
	glUniform1f(diffuseLocation, diffuse);
	glUniform1i(shininessLocation, shininess);


	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &TR[0][0]);
	glBindVertexArray(VAO[Life]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

int Cube::getLife()
{
	return Life;
}

void Cube::MinusLife()
{
	if (Life < 2)
	{
		Life--;
	}
}

float Cube::getzOffset()
{
	return zOffset;
}
float Cube::getRotate()
{
	return rotate;
}

void Cube::set(float l, glm::mat4 pos, glm::mat4 rot)
{
	Life = l;
	PosMat = pos;
	RotMat = rot;

}