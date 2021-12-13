#include "Player.h"
#include "sound.h"

bool Player::loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
) {
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	fclose(file);
	return true;
}


void Player::Init(Player_data pd, Bullet* bullet)
{
	PrevFireTime = std::chrono::system_clock::now();

	for (int i = 0; i < 3; ++i)
	{
		keyDownlist[i] = false;
	}

	PosVec = pd.Posvec;

	Speed = 0.0f;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	loadOBJ("Sphere.obj", vertices, uvs, normals);

	for (int i = 0; i < vertices.size(); ++i)
	{
		Sphere[i][0] = vertices[i].x;
		Sphere[i][1] = vertices[i].y;
		Sphere[i][2] = vertices[i].z;

		SphereNormal[i][0] = normals[i].x;
		SphereNormal[i][1] = normals[i].y;
		SphereNormal[i][2] = normals[i].z;

		SphereColor[i][0] = 1.0f;
		SphereColor[i][1] = 1.0f;
		SphereColor[i][2] = 0.0f;
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), Cube, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), Color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), Normal, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), Indices, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);

	glGenVertexArrays(1, &BulletVAO);
	glBindVertexArray(BulletVAO);
	glGenBuffers(3, BulletVBO);
	glGenBuffers(1, &BulletEBO);

	glBindBuffer(GL_ARRAY_BUFFER, BulletVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 144 * 3 * sizeof(GLfloat), Sphere, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, BulletVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 144 * 3 * sizeof(GLfloat), SphereColor, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, BulletVBO[2]);
	glBufferData(GL_ARRAY_BUFFER, 144 * 3 * sizeof(GLfloat), SphereNormal, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	for (int i = 0; i < MAX_BULLET; i++)
		bullet[i].Init(BulletVAO);

}

void Player::Move(Player_data pd)
{
}

void Player::Update(Player_data pd)
{
	RotMat = pd.RotMat;
	PosVec = pd.Posvec;
	PosMat = pd.PosMat;
	SclMat = pd.SclMat;
	
	camera.setPosition(PosVec.z);
}

void Player::Key_Input(SOCKET sock, unsigned char key, bool state)
{
	if (key == ' ')
	{
		if (state)
		{
			Send_event(sock, CS_FIRE);
		}
	}
}

void Player::sKey_Input(SOCKET sock, int key, bool state)
{
	if (key == GLUT_KEY_RIGHT)
	{
		if (state) {
			Send_event(sock, CS_PLAYER_RIGHT_DOWN); //누름
		}
	
	}

	if (key == GLUT_KEY_LEFT)
	{
		if (state) {
		Send_event(sock, CS_PLAYER_LEFT_DOWN); //누름
	}
		
	}
}

void Player::Render(GLuint ShaderProgram)
{

	camera.Render(ShaderProgram);

	unsigned int modelLocation = glGetUniformLocation(ShaderProgram, "modelTransform");

	glm::mat4 TR;
	TR = RotMat * PosMat * SclMat;

	unsigned int specularLocation = glGetUniformLocation(ShaderProgram, "spec_strength");
	unsigned int diffuseLocation = glGetUniformLocation(ShaderProgram, "diffuse_strength");
	unsigned int shininessLocation = glGetUniformLocation(ShaderProgram, "shininess");

	glUniform1f(specularLocation, specular);
	glUniform1f(diffuseLocation, diffuse);
	glUniform1i(shininessLocation, shininess);

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &TR[0][0]);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	std::vector<Bullet>::iterator iter = BulletList.begin();

	for (; iter != BulletList.end(); ++iter)
	{
		iter->Render(ShaderProgram);
	}
}

void Player::Fire()
{
}

void Player::ManageBullet()
{
}

float Player::getSpeed()
{
	return Speed;
}

float Player::getRotate()
{
	return rad;
}



std::vector<Bullet> Player::getBulletList()
{
}

void Player::Reset()
{
}

void Player::setBulletList(std::vector<Bullet> tmpList)
{
}

glm::vec3 Player::getPosition()
{
	return PosVec;
}

Camera Player::getCamera()
{
	return camera;
}

void Player::setRad(float radian)
{
	rad = radian;
}
