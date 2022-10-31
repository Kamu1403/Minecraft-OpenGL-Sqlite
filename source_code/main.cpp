#include <iostream>
#include "Window.h"
#include "Controller.h"
#include "Cube.h"
#include "Random.h"
#include"DBLink.h"

#include <ctime>


using namespace std;

static WindowBuild* window = WindowBuild::getInstance();
static Loader* loadWorld = Loader::getInstance();
static Control* control = Control::getInstance();
static CubeFlow* renderFlow = CubeFlow::getInstance();
static RandomBlock* world = RandomBlock::getInstance();
Shader Cube::shader = Shader("./resource/Shader/cube.vs", "./resource/Shader/cube.fs");
static BlockMemFlow memFlow("world1", false);

void initAll()
{
	window->skyLoader = loadWorld;
	window->skyLoader->init();
	renderFlow->cube1.init();
	renderFlow->cube2.init();
	renderFlow->cube3.init();
	renderFlow->cube4.init();
	renderFlow->cube5.init();
	renderFlow->cube6.init();
	world->mem = &memFlow;
}

void loadMap(int x, int y, int z)
{
	std::vector<BlockPos> notGenerated = world->mem->testPosition(x, y, z);
	for (size_t i = 0; i < notGenerated.size(); i++)	//模拟生成
	{
		Block* block = new Block;
		world->mapData = block->write();
		world->clear();
		int base_y = notGenerated[i].block_y * BLOCK_LENGTH_Y - (unsigned int)0x80000000;
		if (base_y <= y && base_y + BLOCK_LENGTH_Y > y)
			world->CreateCubeMap();
		world->mem->newPosition(block, notGenerated[i]);
	}
	world->mapData = nullptr;
	world->mem->loadPosition(x, y, z);
}

int main()
{
	initAll();
	cout << "init成功" << endl;
	loadMap(BLOCK_LENGTH_Z / 2, BLOCK_LENGTH_Y - 1, BLOCK_LENGTH_Z / 2);
	cout << "loadMap成功" << endl;
	control->getHeight(world->getHeight(BLOCK_LENGTH_Z / 2, BLOCK_LENGTH_Z / 2, BLOCK_LENGTH_Y - 1));
	while (!glfwWindowShouldClose(window->getWindow()))
	{
		double nowTime = glfwGetTime();
		control->elapseTime = nowTime - control->old;
		control->old = nowTime;
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		control->keyPress();

		loadMap(control->eyePos.x, control->eyePos.y, control->eyePos.z);
		renderFlow->drawMap(control->eyePos);
		control->fall();

		glDisable(GL_CULL_FACE);
		window->skyLoader->drawSkybox();
		glfwSwapBuffers(window->getWindow());
		glfwPollEvents();
	}
	glfwTerminate();
	exit(0);
	return 0;
}
