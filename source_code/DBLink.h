#pragma once

#include "sqlite3.h"
#include"block.h"
#include<vector>
#include <iostream>
#include <string>

/// <summary>
/// 地图坐标转换为区块坐标
/// </summary>
class BlockPos
{
public:
	unsigned int block_x, block_y, block_z;

	BlockPos() {
		/// 假设这个坐标在开始的时候不会被访问到
		block_x = 0;
		block_y = 0;
		block_z = 0;
	}

	BlockPos(int pos_x, int pos_y, int pos_z) {
		block_x = ((unsigned int)pos_x + (unsigned int)0x80000000) / BLOCK_LENGTH_X;
		block_y = ((unsigned int)pos_y + (unsigned int)0x80000000) / BLOCK_LENGTH_Y;
		block_z = ((unsigned int)pos_z + (unsigned int)0x80000000) / BLOCK_LENGTH_Z;
	}

	bool operator==(const BlockPos& block)const {
		return block_x == block.block_x && block_y == block.block_y && block_z == block.block_z;
	}

	const std::string where_clause() const {
		return std::string("where block_x=").append(std::to_string(block_x))
			.append(" and block_y=").append(std::to_string(block_y))
			.append(" and block_z=").append(std::to_string(block_z));
	}

	const std::string values_clause() const {
		return std::string("values(").append(std::to_string(block_x))
			.append(",").append(std::to_string(block_y))
			.append(",").append(std::to_string(block_z).append(",?)"));
	}
};

/// <summary>
/// ***重要：修改后dirty改为True***
/// ？修改算法->增加八叉树场景管理data
/// </summary>
class Block
{
public:
	typedef CUBE_TYPE(*data_type)[BLOCK_LENGTH_Y][BLOCK_LENGTH_Z];

	/// <summary>
	/// 构造
	/// </summary>
	/// <param name="alloc_mem">是否同时分配空间</param>
	Block(bool alloc_mem = true) {
		data = nullptr;
		if (alloc_mem)
			alloc();
		else
			data = nullptr;
		dirty = false;
	}

	void alloc() {
		if (data)
			delete[] data;

		data = new(std::nothrow) CUBE_TYPE[BLOCK_LENGTH_X][BLOCK_LENGTH_Y][BLOCK_LENGTH_Z];
		if (!data)
		{
			std::cout << "内存空间耗尽" << std::endl;
			exit(-1);
		}
	}

	bool isAlloc() const {
		return data;
	}

	~Block() {
		if (data != nullptr)
			delete[] data;
	}

	/// <summary>
	/// 只读数据
	/// </summary>
	/// <returns>只读指针data_type</returns>
	const data_type read() const {
		return (const data_type)data;
	}

	/// <summary>
	/// 修改数据
	/// </summary>
	/// <returns>可改指针data_type</returns>
	data_type write() {
		dirty = true;
		return data;
	}

	bool isDirty() {
		return dirty;
	}

	void clear() {
		dirty = false;
	}

	bool isUse(bool also_clear = false) {
		if (nru_use) {
			if (also_clear)
				nru_use = false;
			return true;
		}
		else
			return false;
	}

	void setUse() {
		nru_use = true;
	}
private:
	/// 顺序x,y,z
	data_type data;

	/// 判断是否修改
	bool dirty;
	/// NRU算法判断位
	bool nru_use;
};


class DBLink
{
public:
	/// <summary>
	/// 构建数据库表项（新的地图）
	/// </summary>
	/// <param name="map_name">新的地图名</param>
	/// <param name="over_write">相同的地图是否覆盖</param>
	void generateMap(const char* map_name, bool over_write);

	/// <summary>
	/// 删除地图
	/// </summary>
	/// <param name="map_name">地图名</param>
	void deleteMap(const char* map_name);

	/// <summary>
	/// 判断地图（名）是否在数据库中
	/// </summary>
	/// <param name="map_name">地图名</param>
	/// <returns>查找成功返回查询到的数量（rc>=0），否则返回-1</returns>
	int isExistMap(const char* map_name);

	/// <summary>
	/// 判断区块是否在数据库中
	/// </summary>
	/// <param name="map_name">地图名</param>
	/// <param name="block">BlockPos生成的区块地址</param>
	/// <returns>查找成功返回查询到的数量（rc>=0），否则返回-1</returns>
	int isExistBlock(const char* map_name, const BlockPos& block);

	/// <summary>
	/// 更新一个已经生成的区块
	/// </summary>
	/// <param name="data">区块数据</param>
	/// <param name="map_name">地图名</param>
	/// <param name="block">BlockPos生成的区块地址</param>
	void updateBlock(const Block& data, const char* map_name, const BlockPos& block);

	/// <summary>
	/// 生成新的区块
	/// </summary>
	/// <param name="data">区块数据</param>
	/// <param name="map_name">地图名</param>
	/// <param name="block">BlockPos生成的区块地址</param>
	void saveBlock(const Block& data, const char* map_name, const BlockPos& block);

	/// <summary>
	/// 从数据库读取区块
	/// </summary>
	/// <param name="data">读取到的区块</param>
	/// <param name="map_name">地图名</param>
	/// <param name="block">BlockPos生成的区块地址</param>
	void loadBlock(Block& data, const char* map_name, const BlockPos& block);

	/// <summary>
	/// 构造函数，连接存储文件。创建地图时，先调用GenerateMap生成数据库表项
	/// </summary>
	/// <param name="db_filename">存储文件名（默认"archive.db"）</param>
	DBLink(const char* db_filename = nullptr);
	~DBLink();

	/// <summary>
	/// 回调函数，显示以及存储输出
	/// </summary>
	/// <param name="NotUsed"></param>
	/// <param name="argc"></param>
	/// <param name="argv"></param>
	/// <param name="azColName"></param>
	/// <returns>返回0</returns>
	static int callback(void* NotUsed, int argc, char** argv, char** azColName);
	static int callback_is_exist(void* NotUsed, int argc, char** argv, char** azColName);

	void test_output(const char* map_name);
private:
	sqlite3* sqlite;
	char* zErrMsg;

	static int callback_result;
};

class BlockMemFlow
{
public:
	/// <summary>
	/// 构造一次
	/// </summary>
	/// <param name="map_name">地图名</param>
	/// <param name="over_write">是否覆盖</param>
	BlockMemFlow(const char* map_name, bool over_write);
	~BlockMemFlow();

	/// <summary>
	/// 返回坐标(x,y,z)周围未生成的区块
	/// </summary>
	/// <returns>未生成BlockPos的向量</returns>
	std::vector<BlockPos> testPosition(int x, int y, int z);

	/// <summary>
	/// 加载坐标(x,y,z)周围的区块到block_load
	/// </summary>
	/// <returns>是否加载成功</returns>
	bool loadPosition(int x, int y, int z);

	/// <summary>
	/// 随机挑选cache换出中增加新区块？改进算法->优先替换NULL和!dirty
	/// </summary>
	/// <param name="data">区块数据Block的指针，绑定后不需要释放，由BlockMemFlow释放</param>
	/// <param name="block">BlockPos生成的区块地址</param>
	void newPosition(Block* data, const BlockPos& block);

	/// <summary>
	/// 获得一个已经载入的区块数据Block的指针
	/// </summary>
	/// <param name="block">BlockPos生成的区块地址</param>
	/// <returns>未载入产生错误，NULL</returns>
	Block* getPosition(const BlockPos& block);

	/// <summary>
	/// 获得一个cache的区块数据Block的指针
	/// </summary>
	/// <param name="block">BlockPos生成的区块地址</param>
	/// <returns>未载入，NULL</returns>
	Block* getCachePosition(const BlockPos& block);

	/// <summary>
	/// 生成坐标(x,y,z)周围加载的区块地址，要求BLOCK_LOAD_NUM为偶数
	/// </summary>
	/// <returns>BlockPos的向量</returns>
	std::vector<BlockPos> adjacentBlock(int x, int y, int z);

	void test_output();

	unsigned const int LOAD_SIZE = BLOCK_LOAD_NUM * BLOCK_LOAD_NUM * BLOCK_LOAD_NUM;
	unsigned const int CACHE_SIZE = (BLOCK_LOAD_NUM + 1) * (BLOCK_LOAD_NUM + 1) * (BLOCK_LOAD_NUM + 1) - BLOCK_LOAD_NUM * BLOCK_LOAD_NUM * BLOCK_LOAD_NUM;

	DBLink db;
	std::string map_name;
	Block* block_load[BLOCK_LOAD_NUM * BLOCK_LOAD_NUM * BLOCK_LOAD_NUM];
	Block* block_cache[(BLOCK_LOAD_NUM + 1) * (BLOCK_LOAD_NUM + 1) * (BLOCK_LOAD_NUM + 1) - BLOCK_LOAD_NUM * BLOCK_LOAD_NUM * BLOCK_LOAD_NUM];
	BlockPos block_pos[(BLOCK_LOAD_NUM + 1) * (BLOCK_LOAD_NUM + 1) * (BLOCK_LOAD_NUM + 1)];

private:
	/// <summary>
	/// 搜索内存中加载的区块
	/// </summary>
	/// <param name="block">BlockPos生成的区块地址</param>
	/// <returns>block_pos中位置，否则返回-1</returns>
	int searchMemBlockPos(const BlockPos& block);

	/// <summary>
	/// 判断区块是否在数据库中
	/// </summary>
	/// <param name="block">BlockPos生成的区块地址</param>
	/// <returns>查找成功返回查询到的数量（rc>=0），否则返回-1</returns>
	int searchDbBlockPos(const BlockPos& block);

	std::vector<BlockPos> outerBlock(int x, int y, int z);

	/// <summary>
	/// 使用NRU算法选择替换出cache的内容，免除项exist[i]!=0
	/// </summary>
	/// <param name="exist">免除项，从0开始</param>
	/// <returns>选择替换出的项目（以block_cache为index）</returns>
	size_t nru_cache_search(const int* exist = nullptr);
	size_t nru_pointer;
};

