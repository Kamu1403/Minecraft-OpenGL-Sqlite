#define _CRT_SECURE_NO_WARNINGS
#include "Cube.h"
#include "Window.h"
#include "Controller.h"

static Control* control = Control::getInstance();
static glm::vec3 lightPos(BLOCK_LENGTH_Z / 2, BLOCK_LENGTH_Y * 5, -BLOCK_LENGTH_Z / 2);

glm::mat4 Cube::projection = glm::perspective(glm::radians(45.0f), WIN_WIDTH / WIN_HEIGHT, 0.1f, 500.0f);
glm::mat4 Cube::view = glm::mat4(1.0f);

// ��ֱ��������
static const float verticesV[] =
{
	// ����
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
};

// ˮƽ��������
static const float verticesH[] =
{
	// ǰ��
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,

	// ����
	-0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

	 0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
};

// ��ֱ��������
static const unsigned int indicesV[] =
{
	// ����
	2, 5, 3,
	3, 5, 7,
	6, 1, 0,
	4, 6, 0,
};

// ˮƽ��������
static const unsigned int indicesH[] =
{
	// ��ǰ
	3,  0,  2,
	1,  2,  0,
	7,  5,  6,
	4,  7,  6,
	// ���� 
	11, 10,  8,
	11,  8,  9,
	13, 14, 15,
	14, 13, 12,
};

Cube::Cube(const char* textureStrV, const char* textureStrH)
{
	this->textureStrV = textureStrV;
	this->textureStrH = textureStrH;
}

Cube::~Cube()
{
}

void Cube::init()
{
	painterVec.resize(maxCnt);

	// �������ݳ�ʼ��
	glGenVertexArrays(1, &VAO_H);
	glGenBuffers(1, &VBO_H);
	glGenBuffers(1, &EBO_H);

	glBindVertexArray(VAO_H);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_H);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesH), verticesH, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_H);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesH), indicesH, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// OpenGL Instantiate
	glGenBuffers(1, &painterVBO_H);
	glBindVertexArray(VAO_H);
	glBindBuffer(GL_ARRAY_BUFFER, painterVBO_H);
	glBufferData(GL_ARRAY_BUFFER, maxCnt * sizeof(glm::mat4), &painterVec[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);


	// ��������
	char texture[60] = "./resource/Textures/";
	strcat(texture, textureStrH);
	textureH = loadTexture(texture);



	// �������ݳ�ʼ��
	glGenVertexArrays(1, &VAO_V);
	glGenBuffers(1, &VBO_V);
	glGenBuffers(1, &EBO_V);

	glBindVertexArray(VAO_V);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_V);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesV), verticesV, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_V);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesV), indicesV, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// OpenGL Instantiate
	glGenBuffers(1, &painterVBO_V);
	glBindVertexArray(VAO_V);
	glBindBuffer(GL_ARRAY_BUFFER, painterVBO_V);
	glBufferData(GL_ARRAY_BUFFER, maxCnt * sizeof(glm::mat4), &painterVec[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	// ��������
	char texturePath1[60] = "./resource/Textures/";
	strcat(texturePath1, textureStrV);
	textureV = loadTexture(texturePath1);


	shader.use();
	shader.setMat4("projection", projection);
}

void Cube::drawStart()
{
	shader.use();
	// ���÷���۲�λ��
	view = glm::lookAt(control->cameraPos, control->cameraPos + control->cameraFront, control->cameraUp);
	shader.setMat4("view", view);

	// �������
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 7.5f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;

	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shader.setInt("diffuseTexture", 0);
	shader.setInt("shadowMap", 1);
	shader.setVec3("viewPos", control->eyePos);
	shader.setVec3("lightPos", lightPos);
}

void Cube::draw()
{
	if (maxCnt == 0)
		return;

	// OpenGL Instantiate
	glBindBuffer(GL_ARRAY_BUFFER, painterVBO_H);
	void* buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	memset(buffer, 0, maxCnt * sizeof(glm::mat4));
	memcpy(buffer, &painterVec[0], maxCnt * sizeof(glm::mat4));
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindTexture(GL_TEXTURE_2D, textureH);
	glBindVertexArray(VAO_H);
	glDrawElementsInstanced(GL_TRIANGLES, sizeof(indicesH), GL_UNSIGNED_INT, 0, maxCnt);


	// OpenGL Instantiate
	glBindBuffer(GL_ARRAY_BUFFER, painterVBO_V);
	buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memset(buffer, 0, maxCnt * sizeof(glm::mat4));
	memcpy(buffer, &painterVec[0], maxCnt * sizeof(glm::mat4));
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindTexture(GL_TEXTURE_2D, textureV);
	glBindVertexArray(VAO_V);
	glDrawElementsInstanced(GL_TRIANGLES, sizeof(indicesV), GL_UNSIGNED_INT, 0, maxCnt);
}

void Cube::push(int x, int y, int z)
{
	maxCnt++;
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(x, y, z));

	painterVec.push_back(model);
}

void Cube::clear()
{
	maxCnt = 0;
	painterVec.clear();
}