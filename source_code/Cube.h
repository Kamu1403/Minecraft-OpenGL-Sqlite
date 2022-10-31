#pragma once

#include "Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

// ����ģ����
class Cube
{
public:
	Cube(const char* textureStrH, const char* textureStrV);
	~Cube();

	// ��ʼ���л���
	static void drawStart();

	void init();
	void draw();
	// ����ɹ۲쵽�ķ��������Ⱦ����
	void push(int x, int y, int z);
	// ��Ⱦ�������
	void clear();

private:

	static Shader shader;
	static glm::mat4 projection;
	static glm::mat4 view;

	// ��ֱ����
	const char* textureStrH;
	// ˮƽ����
	const char* textureStrV;

	// ��ֱ
	unsigned int VAO_V = 0, VBO_V = 0, EBO_V = 0;
	// ˮƽ
	unsigned int VAO_H = 0, VBO_H = 0, EBO_H = 0;
	// ����
	unsigned int textureH = 0, textureV = 0;
	// ��Ҫ��Ⱦ�ĸ���
	int maxCnt = 50000;
	// ���㻺��������
	unsigned int painterVBO_H = 0, painterVBO_V = 0;
	// OpenGL Instantiate ��Ⱦ����
	std::vector<glm::mat4> painterVec;
};

#include "Random.h"

#define VISION_WIDTH	30		// ��Ұ���
#define VISION_HEIGHT	30		// ��Ұ�߶�

class CubeFlow
{
public:
	static CubeFlow* getInstance();
	// ����ͼ
	void drawMap(glm::vec3 vec);

	Cube cube1 = Cube("dirt.png", "dirt.png");
	Cube cube2 = Cube("sand.png", "sand.png");
	Cube cube3 = Cube("stone.png", "stone.png");
	Cube cube4 = Cube("planks_oak.png", "planks_oak.png");
	Cube cube5 = Cube("cobblestone.png", "cobblestone.png");
	Cube cube6 = Cube("wool_colored_lime.png", "dirt.png");

private:
	CubeFlow() = default;

	// ����ͼ������Ԫ��
	void drawAll();
	// ��ջ���
	void clearAll();
	// �������
	void push(CubeType ct, int i, int j, int k);
	// �жϷ����Ƿ�¶���Ⲣ����
	inline bool peripheral(const Block::data_type data, int x, int y, int z);
};