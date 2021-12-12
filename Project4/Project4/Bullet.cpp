#include "Bullet.h"

void Bullet::Init(GLuint vao)
{

	VAO = vao;
}

void Bullet::Render(GLuint ShaderProgram)
{
	if (PosMat != glm::mat4(1.0f)) {
		unsigned int modelLocation = glGetUniformLocation(ShaderProgram, "modelTransform");

		glm::mat4 TR;
		TR = PosMat;

		TR = glm::scale(TR, glm::vec3(0.5f, 0.5f, 0.5f));

		unsigned int specularLocation = glGetUniformLocation(ShaderProgram, "spec_strength");
		unsigned int diffuseLocation = glGetUniformLocation(ShaderProgram, "diffuse_strength");
		unsigned int shininessLocation = glGetUniformLocation(ShaderProgram, "shininess");

		glUniform1f(specularLocation, specular);
		glUniform1f(diffuseLocation, diffuse);
		glUniform1i(shininessLocation, shininess);


		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &TR[0][0]);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 144);
	}
}

void Bullet::Move()
{
	
}

void Bullet::setposMat(glm::mat4 posmat) {
	PosMat = posmat;
};

float Bullet::getzOffset()
{
	return PosVec.z;
}

float Bullet::getRotate()
{
	return rotate;
}
