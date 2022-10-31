#pragma once

// 方块类型
enum class CubeType
{
	NONE,			//空 0
	DIRT,			//土块 1
	SAND,			//沙子 2
	STONE,			//石头 3
	WOOD,		//橡树木板 4
	COBBLE,	//圆石	5
	GRASS //带草土块
};


/// 区块边长 默认支持X=Z Y(altitude)
#define BLOCK_LENGTH_X 32
#define BLOCK_LENGTH_Y 64
#define BLOCK_LENGTH_Z 32

/// 方块种类表示
#define CUBE_TYPE CubeType

/// 载入区块个数，设置为2,4,6...（对应8,64,216）
#define BLOCK_LOAD_NUM 2

