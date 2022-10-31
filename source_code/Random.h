#pragma once

#include"DBlink.h"
#include <glad/glad.h>
#include <vector>
#include <iostream>

using namespace std;

#define getRemain(x,y) ((x%y)<0?(x%y+y):x%y)


// ���������ͼ��
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
	//��ͼ���ɶ���
	CUBE_TYPE(*mapData)[BLOCK_LENGTH_Y][BLOCK_LENGTH_Z];
	BlockMemFlow* mem;

private:
	RandomBlock();

	// ǰ���� (���������ͼʱʹ��)
	vector<int> lineAlt;
	// ��ͼ���� (���������ͼʱʹ��)
	vector<vector<int>> mapAlt;

	// ��ͼ��һ�к������ݵĳ�ʼ��
	void randLineAltitude();
	// �����ʼ����ͼ�����к���
	void uniformAltTotal();
	// ��ͼ����ƽ����
	void avgAltitude();
	// ���ݵ�
	void fillIn();

public:
	inline bool judgeCube(int x, int y, int z, CUBE_TYPE cube) {
		return cube == getCube(x, y, z);
	}
	/// ����load�еķ�����
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
