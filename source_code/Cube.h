#pragma once

#include "Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

// 方块模型类
class Cube
{
public:
	Cube(const char* textureStrH, const char* textureStrV);
	~Cube();

	// 开始集中绘制
	static void drawStart();

	void init();
	void draw();
	// 相机可观察到的方块加入渲染集合
	void push(int x, int y, int z);
	// 渲染集合清空
	void clear();

private:

	static Shader shader;
	static glm::mat4 projection;
	static glm::mat4 view;

	// 垂直纹理
	const char* textureStrH;
	// 水平纹理
	const char* textureStrV;

	// 垂直
	unsigned int VAO_V = 0, VBO_V = 0, EBO_V = 0;
	// 水平
	unsigned int VAO_H = 0, VBO_H = 0, EBO_H = 0;
	// 纹理
	unsigned int textureH = 0, textureV = 0;
	// 需要渲染的个数
	int maxCnt = 50000;
	// 顶点缓冲区对象
	unsigned int painterVBO_H = 0, painterVBO_V = 0;
	// OpenGL Instantiate 渲染集合
	std::vector<glm::mat4> painterVec;
};

#include "Random.h"

#define VISION_WIDTH	30		// 视野宽度
#define VISION_HEIGHT	30		// 视野高度

class CubeFlow
{
public:
	static CubeFlow* getInstance();
	// 画地图
	void drawMap(glm::vec3 vec);

	Cube cube1 = Cube("dirt.png", "dirt.png");
	Cube cube2 = Cube("sand.png", "sand.png");
	Cube cube3 = Cube("stone.png", "stone.png");
	Cube cube4 = Cube("planks_oak.png", "planks_oak.png");
	Cube cube5 = Cube("cobblestone.png", "cobblestone.png");
	Cube cube6 = Cube("wool_colored_lime.png", "dirt.png");

private:
	CubeFlow() = default;

	// 画地图中所有元素
	void drawAll();
	// 清空绘制
	void clearAll();
	// 分类绘制
	void push(CubeType ct, int i, int j, int k);
	// 判断方块是否暴露在外并绘制
	inline bool peripheral(const Block::data_type data, int x, int y, int z);
};