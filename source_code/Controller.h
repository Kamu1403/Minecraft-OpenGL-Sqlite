#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Random.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

// ��һ�˳�
#define EYE_HEIGHT 1

// ����״̬
enum class Status
{
	GROUND,
	UP,
	DOWN
};

// OpenGL������¼�
void mousePress(GLFWwindow* window, int button, int action, int mods);
// OpenGL����ƶ��¼�
void mouseMove(GLFWwindow* window, double xpos, double ypos);

// ��ҿ�����
class Control
{
public:
	static Control* getInstance();
	// ���̻ص�
	void keyPress();
	// �������ʱ�����ڵĺ���
	void getHeight(int alt);
	// �й��������ں��εĴ�����(��Ծ,�½�)
	void fall();
	// ����
	void placeBlock();
	// ����
	void destroyBlock();

	float xz_rad = -270;
	float xy_rad = 0.0f;
	//�ֳַ���
	CubeType nowPlaceCube = CubeType::DIRT;

	// ����״̬
	Status ps = Status::GROUND;
	// ��������
	float beginJumpAltitude = 0;

	// �ӽǺ���
	float height = 0;
	glm::vec3 eyePos = glm::vec3(BLOCK_LENGTH_Z / 2, height, BLOCK_LENGTH_Z / 2);
	glm::vec3 cameraPos = glm::vec3(BLOCK_LENGTH_Z / 2, height, BLOCK_LENGTH_Z / 2 + 0.25f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// ����ʱ��
	double elapseTime = 0.0f;
	// �ϴ�ʱ��
	double old = 1.0f;

private:
	Control();

	// �����¼�
	void windowKey();
	// �����ƶ�
	void cameraMoveKey();
	// ������Ծ
	void cameraJumpKey();
	// �л��ֳַ���
	void takeCubeKey();
	// ��ײ���
	bool isCollision(float x, float y, float z);
};