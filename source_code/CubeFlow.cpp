#include "Cube.h"
#include "Controller.h"
#include "Window.h"

static RandomBlock* world = RandomBlock::getInstance();
static Control* control = Control::getInstance();
static WindowBuild* window = WindowBuild::getInstance();

CubeFlow* CubeFlow::getInstance()
{
	static CubeFlow painter;
	return &painter;
}

float frustumPlane[6][4];

void calculateFrustumPlanes(void)
{
	float p[16];   // projection matrix
	float mv[16];  // model-view matrix
	float mvp[16]; // model-view-projection matrix
	float t;

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			p[i * 4 + j] = window->skyLoader->projection[i][j];
			mv[i * 4 + j] = window->skyLoader->view[i][j];
		}
	}
	//
	// Concatenate the projection matrix and the model-view matrix to produce
	// a combined model-view-projection matrix.
	//

	mvp[0] = mv[0] * p[0] + mv[1] * p[4] + mv[2] * p[8] + mv[3] * p[12];
	mvp[1] = mv[0] * p[1] + mv[1] * p[5] + mv[2] * p[9] + mv[3] * p[13];
	mvp[2] = mv[0] * p[2] + mv[1] * p[6] + mv[2] * p[10] + mv[3] * p[14];
	mvp[3] = mv[0] * p[3] + mv[1] * p[7] + mv[2] * p[11] + mv[3] * p[15];

	mvp[4] = mv[4] * p[0] + mv[5] * p[4] + mv[6] * p[8] + mv[7] * p[12];
	mvp[5] = mv[4] * p[1] + mv[5] * p[5] + mv[6] * p[9] + mv[7] * p[13];
	mvp[6] = mv[4] * p[2] + mv[5] * p[6] + mv[6] * p[10] + mv[7] * p[14];
	mvp[7] = mv[4] * p[3] + mv[5] * p[7] + mv[6] * p[11] + mv[7] * p[15];

	mvp[8] = mv[8] * p[0] + mv[9] * p[4] + mv[10] * p[8] + mv[11] * p[12];
	mvp[9] = mv[8] * p[1] + mv[9] * p[5] + mv[10] * p[9] + mv[11] * p[13];
	mvp[10] = mv[8] * p[2] + mv[9] * p[6] + mv[10] * p[10] + mv[11] * p[14];
	mvp[11] = mv[8] * p[3] + mv[9] * p[7] + mv[10] * p[11] + mv[11] * p[15];

	mvp[12] = mv[12] * p[0] + mv[13] * p[4] + mv[14] * p[8] + mv[15] * p[12];
	mvp[13] = mv[12] * p[1] + mv[13] * p[5] + mv[14] * p[9] + mv[15] * p[13];
	mvp[14] = mv[12] * p[2] + mv[13] * p[6] + mv[14] * p[10] + mv[15] * p[14];
	mvp[15] = mv[12] * p[3] + mv[13] * p[7] + mv[14] * p[11] + mv[15] * p[15];



	//
	// Extract the frustum's right clipping plane and normalize it.
	//

	frustumPlane[0][0] = mvp[3] - mvp[0];
	frustumPlane[0][1] = mvp[7] - mvp[4];
	frustumPlane[0][2] = mvp[11] - mvp[8];
	frustumPlane[0][3] = mvp[15] - mvp[12];

	t = (float)sqrt(frustumPlane[0][0] * frustumPlane[0][0] +
		frustumPlane[0][1] * frustumPlane[0][1] +
		frustumPlane[0][2] * frustumPlane[0][2]);

	frustumPlane[0][0] /= t;
	frustumPlane[0][1] /= t;
	frustumPlane[0][2] /= t;
	frustumPlane[0][3] /= t;

	//
	// Extract the frustum's left clipping plane and normalize it.
	//

	frustumPlane[1][0] = mvp[3] + mvp[0];
	frustumPlane[1][1] = mvp[7] + mvp[4];
	frustumPlane[1][2] = mvp[11] + mvp[8];
	frustumPlane[1][3] = mvp[15] + mvp[12];

	t = (float)sqrt(frustumPlane[1][0] * frustumPlane[1][0] +
		frustumPlane[1][1] * frustumPlane[1][1] +
		frustumPlane[1][2] * frustumPlane[1][2]);

	frustumPlane[1][0] /= t;
	frustumPlane[1][1] /= t;
	frustumPlane[1][2] /= t;
	frustumPlane[1][3] /= t;



	//
	// Extract the frustum's bottom clipping plane and normalize it.
	//

	frustumPlane[2][0] = mvp[3] + mvp[1];
	frustumPlane[2][1] = mvp[7] + mvp[5];
	frustumPlane[2][2] = mvp[11] + mvp[9];
	frustumPlane[2][3] = mvp[15] + mvp[13];

	t = (float)sqrt(frustumPlane[2][0] * frustumPlane[2][0] +
		frustumPlane[2][1] * frustumPlane[2][1] +
		frustumPlane[2][2] * frustumPlane[2][2]);

	frustumPlane[2][0] /= t;
	frustumPlane[2][1] /= t;
	frustumPlane[2][2] /= t;
	frustumPlane[2][3] /= t;

	//
	// Extract the frustum's top clipping plane and normalize it.
	//

	frustumPlane[3][0] = mvp[3] - mvp[1];
	frustumPlane[3][1] = mvp[7] - mvp[5];
	frustumPlane[3][2] = mvp[11] - mvp[9];
	frustumPlane[3][3] = mvp[15] - mvp[13];

	t = (float)sqrt(frustumPlane[3][0] * frustumPlane[3][0] +
		frustumPlane[3][1] * frustumPlane[3][1] +
		frustumPlane[3][2] * frustumPlane[3][2]);

	frustumPlane[3][0] /= t;
	frustumPlane[3][1] /= t;
	frustumPlane[3][2] /= t;
	frustumPlane[3][3] /= t;



	//
	// Extract the frustum's far clipping plane and normalize it.
	//

	frustumPlane[4][0] = mvp[3] - mvp[2];
	frustumPlane[4][1] = mvp[7] - mvp[6];
	frustumPlane[4][2] = mvp[11] - mvp[10];
	frustumPlane[4][3] = mvp[15] - mvp[14];

	t = (float)sqrt(frustumPlane[4][0] * frustumPlane[4][0] +
		frustumPlane[4][1] * frustumPlane[4][1] +
		frustumPlane[4][2] * frustumPlane[4][2]);

	frustumPlane[4][0] /= t;
	frustumPlane[4][1] /= t;
	frustumPlane[4][2] /= t;
	frustumPlane[4][3] /= t;

	//
	// Extract the frustum's near clipping plane and normalize it.
	//

	frustumPlane[5][0] = mvp[3] + mvp[2];
	frustumPlane[5][1] = mvp[7] + mvp[6];
	frustumPlane[5][2] = mvp[11] + mvp[10];
	frustumPlane[5][3] = mvp[15] + mvp[14];

	t = (float)sqrt(frustumPlane[5][0] * frustumPlane[5][0] +
		frustumPlane[5][1] * frustumPlane[5][1] +
		frustumPlane[5][2] * frustumPlane[5][2]);

	frustumPlane[5][0] /= t;
	frustumPlane[5][1] /= t;
	frustumPlane[5][2] /= t;
	frustumPlane[5][3] /= t;

}

bool isBoundingSphereInFrustum(float x, float y, float z)
{
	for (int i = 0; i < 6; ++i)
	{
		if (frustumPlane[i][0] * x +
			frustumPlane[i][1] * y +
			frustumPlane[i][2] * z +
			frustumPlane[i][3] <= -1)
			return false;
	}
	return true;
}

void CubeFlow::drawMap(glm::vec3 cameraPos)
{
	clearAll();

	// ÊÓÒ°
	int begin_x = cameraPos.x - VISION_WIDTH;
	int end_x = cameraPos.x + VISION_WIDTH;
	int begin_y = cameraPos.y - VISION_HEIGHT;
	int end_y = cameraPos.y + VISION_HEIGHT;
	int begin_z = cameraPos.z - VISION_WIDTH;
	int end_z = cameraPos.z + VISION_WIDTH;

	calculateFrustumPlanes();
	for (size_t i = 0; i < world->mem->LOAD_SIZE + world->mem->CACHE_SIZE; i++)
	{
		Block* cur;
		BlockPos pos = world->mem->block_pos[i];
		int base_x = pos.block_x * BLOCK_LENGTH_X - (unsigned int)0x80000000;
		int base_y = pos.block_y * BLOCK_LENGTH_Y - (unsigned int)0x80000000;
		int base_z = pos.block_z * BLOCK_LENGTH_Z - (unsigned int)0x80000000;
		if (i < world->mem->LOAD_SIZE)
		{
			cur = world->mem->block_load[i];
		}
		else
		{
			cur = world->mem->block_cache[i - world->mem->LOAD_SIZE];
		}
		if (cur == nullptr)
		{
			continue;
		}
		const Block::data_type data = cur->read();
		for (size_t j = 0; j < BLOCK_LENGTH_X; j++)
		{
			for (size_t k = 0; k < BLOCK_LENGTH_Y; k++)
			{
				for (size_t l = 0; l < BLOCK_LENGTH_Z; l++)
				{
					if (isBoundingSphereInFrustum(base_x + j, base_y + k, base_z + l))
					{
						if (j == 0 || j == BLOCK_LENGTH_X - 1 || k == 0 || k == BLOCK_LENGTH_Y - 1 || l == 0 || l == BLOCK_LENGTH_Z)
						{
							if (i < world->mem->LOAD_SIZE)
								push(data[j][k][l], base_x + j, base_y + k, base_z + l);
							else
							{
								CUBE_TYPE cube = data[j][k][l];
								if (cube != CubeType::NONE)
									push(CubeType(int(cube) - 1), base_x + j, base_y + k, base_z + l);
							}
						}
						else
						{
							if (peripheral(data, j, k, l)) {
								if (i < world->mem->LOAD_SIZE)
									push(data[j][k][l], base_x + j, base_y + k, base_z + l);
								else
								{
									CUBE_TYPE cube = data[j][k][l];
									if (cube != CubeType::NONE)
										push(CubeType(int(cube) - 1), base_x + j, base_y + k, base_z + l);
								}
							}
						}
					}
				}
			}
		}
	}
	drawAll();
}

void CubeFlow::drawAll()
{
	// ¿ªÆôÃæÌÞ³ý
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	Cube::drawStart();
	cube1.draw();
	cube2.draw();
	cube3.draw();
	cube4.draw();
	cube5.draw();
	cube6.draw();
}

void CubeFlow::clearAll()
{
	cube1.clear();
	cube2.clear();
	cube3.clear();
	cube4.clear();
	cube5.clear();
	cube6.clear();
}

void CubeFlow::push(CubeType ct, int i, int j, int k)
{
	switch (ct)
	{
		case CubeType::NONE:
			break;
		case CubeType::DIRT:
			this->cube1.push(i, j, k);
			break;
		case CubeType::SAND:
			this->cube2.push(i, j, k);
			break;
		case CubeType::STONE:
			this->cube3.push(i, j, k);
			break;
		case CubeType::WOOD:
			this->cube4.push(i, j, k);
			break;
		case CubeType::COBBLE:
			this->cube5.push(i, j, k);
			break;
		case CubeType::GRASS:
			this->cube6.push(i, j, k);
			break;
		default:
			break;
	}
}

inline bool CubeFlow::peripheral(const Block::data_type data, int x, int y, int z)
{
	return  data[x + 1][y][z] == CubeType::NONE ||
		data[x - 1][y][z] == CubeType::NONE ||
		data[x][y + 1][z] == CubeType::NONE ||
		data[x][y - 1][z] == CubeType::NONE ||
		data[x][y][z + 1] == CubeType::NONE ||
		data[x][y][z - 1] == CubeType::NONE;
}
