#pragma once

// ��������
enum class CubeType
{
	NONE,			//�� 0
	DIRT,			//���� 1
	SAND,			//ɳ�� 2
	STONE,			//ʯͷ 3
	WOOD,		//����ľ�� 4
	COBBLE,	//Բʯ	5
	GRASS //��������
};


/// ����߳� Ĭ��֧��X=Z Y(altitude)
#define BLOCK_LENGTH_X 32
#define BLOCK_LENGTH_Y 64
#define BLOCK_LENGTH_Z 32

/// ���������ʾ
#define CUBE_TYPE CubeType

/// �����������������Ϊ2,4,6...����Ӧ8,64,216��
#define BLOCK_LOAD_NUM 2

