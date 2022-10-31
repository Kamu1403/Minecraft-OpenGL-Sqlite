#pragma once

#include"DBlink.h"
#include <glad/glad.h>
#include <vector>
#include <iostream>

using namespace std;

#define getRemain(x,y) ((x%y)<0?(x%y+y):x%y)


// 随机创建地图类
class RandomBlock
{
public:
	static RandomBlock* getInstance();
	void clear();
	void CreateCubeMap();
	int getHeight(int x, int z, int y) {
		for (int i = y; i > 0; i--)
		{
			if (!this->judgeCube(x, i, z, CubeType::NONE))
				return i + 1;
		}
		return 0;
	}
public:
	//地图生成对象
	CUBE_TYPE(*mapData)[BLOCK_LENGTH_Y][BLOCK_LENGTH_Z];
	BlockMemFlow* mem;

private:
	RandomBlock();

	// 前海拔 (创建随机地图时使用)
	vector<int> lineAlt;
	// 地图海拔 (创建随机地图时使用)
	vector<vector<int>> mapAlt;

	// 地图中一行海拔数据的初始化
	void randLineAltitude();
	// 随机初始化地图的所有海拔
	void uniformAltTotal();
	// 地图海拔平均化
	void avgAltitude();
	// 填充草地
	void fillIn();

public:
	inline bool judgeCube(int x, int y, int z, CUBE_TYPE cube) {
		return cube == getCube(x, y, z);
	}
	/// 不在load中的反向处理
	CUBE_TYPE getCube(int x, int y, int z) {
		Block* block = this->mem->getPosition(BlockPos(x, y, z));
		if (block != nullptr)
			return block->read()[getRemain(x, BLOCK_LENGTH_Z)][getRemain(y, BLOCK_LENGTH_Y)][getRemain(z, BLOCK_LENGTH_Z)];
		else
		{
			block = this->mem->getCachePosition(BlockPos(x, y, z));
			if (block != nullptr) {
				CUBE_TYPE cube = block->read()[getRemain(x, BLOCK_LENGTH_Z)][getRemain(y, BLOCK_LENGTH_Y)][getRemain(z, BLOCK_LENGTH_Z)];
				if (cube != CubeType::NONE)
					return CubeType(int(cube) - 1);
			}
		}
		return CubeType::NONE;
	}
	inline void writeCube(int x, int y, int z, CUBE_TYPE cube) {
		Block::data_type data = this->mem->getPosition(BlockPos(x, y, z))->write();
		data[getRemain(x, BLOCK_LENGTH_Z)][getRemain(y, BLOCK_LENGTH_Y)][getRemain(z, BLOCK_LENGTH_Z)] = cube;
	}
};
