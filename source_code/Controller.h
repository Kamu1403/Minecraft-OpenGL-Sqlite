#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Random.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

// 第一人称
#define EYE_HEIGHT 1

// 人物状态
enum class Status
{
	GROUND,
	UP,
	DOWN
};

// OpenGL鼠标点击事件
void mousePress(GLFWwindow* window, int button, int action, int mods);
// OpenGL鼠标移动事件
void mouseMove(GLFWwindow* window, double xpos, double ypos);

// 玩家控制类
class Control
{
public:
	static Control* getInstance();
	// 键盘回调
	void keyPress();
	// 人物出现时的所在的海拔
	void getHeight(int alt);
	// 有关人物所在海拔的处理函数(跳跃,下降)
	void fall();
	// 放置
	void placeBlock();
	// 消除
	void destroyBlock();

	float xz_rad = -270;
	float xy_rad = 0.0f;
	//手持方块
	CubeType nowPlaceCube = CubeType::DIRT;

	// 人物状态
	Status ps = Status::GROUND;
	// 起跳海拔
	float beginJumpAltitude = 0;

	// 视角海拔
	float height = 0;
	glm::vec3 eyePos = glm::vec3(BLOCK_LENGTH_Z / 2, height, BLOCK_LENGTH_Z / 2);
	glm::vec3 cameraPos = glm::vec3(BLOCK_LENGTH_Z / 2, height, BLOCK_LENGTH_Z / 2 + 0.25f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// 经过时间
	double elapseTime = 0.0f;
	// 上次时间
	double old = 1.0f;

private:
	Control();

	// 窗体事件
	void windowKey();
	// 人物移动
	void cameraMoveKey();
	// 人物跳跃
	void cameraJumpKey();
	// 切换手持方块
	void takeCubeKey();
	// 碰撞检测
	bool isCollision(float x, float y, float z);
};