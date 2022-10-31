#include "Random.h"
#include <ctime>
#include <random>


RandomBlock* RandomBlock::getInstance()
{
	static RandomBlock world;
	return &world;
}

RandomBlock::RandomBlock()
{
}

void RandomBlock::CreateCubeMap()
{
	lineAlt.reserve(BLOCK_LENGTH_Z);
	for (int i = 0; i < BLOCK_LENGTH_Z; i++)
		lineAlt.push_back(BLOCK_LENGTH_Y / 2);

	uniformAltTotal();
	avgAltitude();
	fillIn();
}

void RandomBlock::clear()
{
	if (mapData != nullptr)
		for (size_t i = 0; i < BLOCK_LENGTH_Z; i++)
			for (size_t j = 0; j < BLOCK_LENGTH_Y; j++)
				for (size_t k = 0; k < BLOCK_LENGTH_Z; k++)
					mapData[i][j][k] = CubeType::NONE;

}

static std::random_device rd; //一个非确定的种子
static std::default_random_engine e1(rd());//默认随机数生成器
static std::uniform_int_distribution<int> uniform_altitude(-1, 1); //离散均匀分布类
static std::uniform_int_distribution<int> uniform_value(0, BLOCK_LENGTH_Z - 1);

void RandomBlock::randLineAltitude()
{
	int rand_num = 0;
	int altitude = 0;
	double avg1 = 0;
	double avg2 = 0;
	for (int i = 1; i < BLOCK_LENGTH_Z - 1; i++)
	{
		rand_num = uniform_altitude(e1);
		avg1 = double(lineAlt[i] + lineAlt[i - 1] + lineAlt[i + 1]) / 3;
		avg2 = double(avg1 + rand_num + lineAlt[i - 1] + lineAlt[i + 1]) / 3;

		altitude = avg2 + (avg1 < avg2);

		if (altitude <= 0 || altitude >= 200)
			altitude = 100;

		lineAlt[i] = altitude;
	}
}

void RandomBlock::uniformAltTotal()
{
	for (int x = 0; x < BLOCK_LENGTH_Z; x++)
	{
		randLineAltitude();
		mapAlt.push_back(lineAlt);
	}
}

void RandomBlock::avgAltitude()
{
	int avg = 0;
	for (int x = 0; x < BLOCK_LENGTH_Z; x++)
	{
		for (int z = BLOCK_LENGTH_Z; z > 0; z--)
		{
			if (x - 1 >= 0 && x + 1 < BLOCK_LENGTH_Z && z - 1 >= 0 && z + 1 < BLOCK_LENGTH_Z)
				mapAlt[x][z] = (mapAlt[x - 1][z] + mapAlt[x + 1][z] + mapAlt[x][z - 1] + mapAlt[x][z + 1] + mapAlt[x][z]) / 5;
			else if (z < BLOCK_LENGTH_Z)
				mapAlt[x][z] += (min(BLOCK_LENGTH_Z - x, x) + min(BLOCK_LENGTH_Z - z, z)) / 30;
		}
	}
}

void RandomBlock::fillIn()
{
	for (int x = 0; x < BLOCK_LENGTH_Z; x++)
	{
		for (int z = 0; z < BLOCK_LENGTH_Z; z++)
		{
			mapData[x][mapAlt[x][z]][z] = CubeType::GRASS;
			for (int y = 0; y < mapAlt[x][z]; y++)
				mapData[x][y][z] = CubeType::STONE;
		}
	}
}

