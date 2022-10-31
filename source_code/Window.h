#pragma once

#include "Shader.h"
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

// 窗体宽高
static float WIN_WIDTH = 700;
static float WIN_HEIGHT = 400;


// 天空盒模型
class Loader
{
public:
	static Loader* getInstance();
	void init();
	void drawSkybox();

private:
	Loader();
	~Loader();
	Loader(const Loader&) = delete;
	Loader& operator = (const Loader&) = delete;

	unsigned int load(vector<string> faces);

	Shader skyboxShader = Shader("./resource/Shader/sky.vs", "./resource/Shader/sky.fs");

	unsigned int skyboxVAO = 0;
	unsigned int cubemapTexture = 0;

public:
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), WIN_WIDTH / WIN_HEIGHT, 0.1f, 2000.0f);
	glm::mat4 view = glm::mat4(1.0f);
};


class WindowBuild
{
public:
	static WindowBuild* getInstance();
	GLFWwindow* getWindow();
	~WindowBuild();
private:
	WindowBuild();

	GLFWwindow* window = nullptr;
public:
	Loader* skyLoader = nullptr;
};

unsigned int loadTexture(const char* path);
