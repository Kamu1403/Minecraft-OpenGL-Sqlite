#include"DBLink.h"

int DBLink::callback_result;

DBLink::DBLink(const char* db_filename) {
	zErrMsg = nullptr;
	int rc;

	/* Open database */
	if (db_filename)
		rc = sqlite3_open(db_filename, &sqlite);
	else
		rc = sqlite3_open("archive.db", &sqlite);

	if (rc == SQLITE_OK)
		std::cout << "Opened database successfully" << std::endl;
	else
		std::cout << "Can't open database: " << sqlite3_errmsg(sqlite) << std::endl;
}

DBLink::~DBLink() {
	/* Close SQL */
	sqlite3_close(sqlite);
}

void DBLink::generateMap(const char* map_name, bool over_write) {
	int rc;
	std::string sql_cmd;

	int map_exist = isExistMap(map_name);
	if (map_exist == -1)
		return;

	if (map_exist)
	{
		if (over_write)
			deleteMap(map_name);
		else
		{
			std::cout << "Comment: Map already exists in database" << std::endl;
			return;
		}
	}

	/* Execute SQL statement */
	sql_cmd = std::string().append("create table '").append(map_name).append("'(block_x int, block_y int, block_z int, block_data blob)").append(";");
	rc = sqlite3_exec(sqlite, sql_cmd.c_str(), callback, 0, &zErrMsg);
	if (rc == SQLITE_OK)
		std::cout << "Table created successfully" << std::endl;
	else {
		std::cout << "SQL error : " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}
}

void DBLink::deleteMap(const char* map_name) {
	int rc;
	std::string sql_cmd;

	/* Execute SQL statement - drop*/
	sql_cmd = std::string().append("drop table '").append(map_name).append("';");
	rc = sqlite3_exec(sqlite, sql_cmd.c_str(), callback, 0, &zErrMsg);
	if (rc == SQLITE_OK)
		std::cout << "Table drop successfully" << std::endl;
	else {
		std::cout << "SQL error : " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}

}

int DBLink::isExistMap(const char* map_name) {
	int rc;
	std::string sql_cmd;

	/* Execute SQL statement */
	sql_cmd = std::string().append("select count(*) from sqlite_master where type='table' and name='").append(map_name).append("';");
	rc = sqlite3_exec(sqlite, sql_cmd.c_str(), DBLink::callback_is_exist, 0, &zErrMsg);
	if (rc == SQLITE_OK) {
		std::cout << "Map search successfully" << std::endl;
		return DBLink::callback_result;
	}
	else {
		std::cout << "SQL error : " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
		return -1;
	}
}

int DBLink::isExistBlock(const char* map_name, const BlockPos& block) {
	int rc;
	std::string sql_cmd;

	/* Execute SQL statement */
	sql_cmd = std::string().append("select count(*) from '").append(map_name).append("' ").append(block.where_clause()).append(";");
	rc = sqlite3_exec(sqlite, sql_cmd.c_str(), DBLink::callback_is_exist, 0, &zErrMsg);
	if (rc == SQLITE_OK) {
		std::cout << "Table search successfully" << std::endl;
		return DBLink::callback_result;
	}
	else {
		std::cout << "SQL error : " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
		return -1;
	}
}

void DBLink::updateBlock(const Block& data, const char* map_name, const BlockPos& block) {
	int rc;
	std::string sql_cmd;
	sqlite3_stmt* stmt;

	/* Execute SQL statement */
	sql_cmd = std::string().append("update '").append(map_name).append("' set block_data=? ").append(block.where_clause()).append(";");
	rc = sqlite3_prepare(sqlite, sql_cmd.c_str(), sql_cmd.length(), &stmt, nullptr);
	if (rc == SQLITE_OK)
	{
		rc = sqlite3_bind_blob(stmt, 1, data.read(), sizeof(CUBE_TYPE) * BLOCK_LENGTH_X * BLOCK_LENGTH_Y * BLOCK_LENGTH_Z, NULL);
		if (rc == SQLITE_OK) {
			sqlite3_step(stmt);
			std::cout << "Table update successfully" << std::endl;
		}
	}
	else
		std::cout << "SQL error : " << sqlite3_errmsg(sqlite) << std::endl;

	sqlite3_finalize(stmt);
}

void DBLink::saveBlock(const Block& data, const char* map_name, const BlockPos& block) {
	int rc;
	std::string sql_cmd;
	sqlite3_stmt* stmt;

	/* Execute SQL statement */
	sql_cmd = std::string().append("insert into '").append(map_name).append("' ").append(block.values_clause()).append(";");
	rc = sqlite3_prepare(sqlite, sql_cmd.c_str(), sql_cmd.length(), &stmt, nullptr);
	if (rc == SQLITE_OK)
	{
		rc = sqlite3_bind_blob(stmt, 1, data.read(), sizeof(CUBE_TYPE) * BLOCK_LENGTH_X * BLOCK_LENGTH_Y * BLOCK_LENGTH_Z, NULL);
		if (rc == SQLITE_OK) {
			if (sqlite3_step(stmt) == SQLITE_DONE)
				std::cout << "Table insert successfully" << std::endl;
			else
				std::cout << "SQL error : " << sqlite3_errmsg(sqlite) << std::endl;
		}
	}
	else
		std::cout << "SQL error : " << sqlite3_errmsg(sqlite) << std::endl;

	sqlite3_finalize(stmt);
}


void DBLink::loadBlock(Block& data, const char* map_name, const BlockPos& block) {
	int rc;
	std::string sql_cmd;
	sqlite3_stmt* stmt;

	if (!data.isAlloc())
		data.alloc();

	/* Execute SQL statement */
	sql_cmd = std::string().append("select block_data from '").append(map_name).append("' ").append(block.where_clause()).append(";");
	rc = sqlite3_prepare(sqlite, sql_cmd.c_str(), sql_cmd.length(), &stmt, nullptr);
	if (rc == SQLITE_OK)
	{
		rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW) {
			const void* pReadBolbData = sqlite3_column_blob(stmt, 0);
			std::cout << "Table search successfully" << std::endl;
			memcpy(data.write(), pReadBolbData, sizeof(CUBE_TYPE) * BLOCK_LENGTH_X * BLOCK_LENGTH_Y * BLOCK_LENGTH_Z);
			data.clear();	/// 刚读出的数据，清空dirty
		}
		else
			std::cout << "Error：未找到当前区块，坐标为（" << block.block_x << "，" << block.block_y << "，" << block.block_z << "）" << std::endl;
	}
	else
		std::cout << "SQL error : " << sqlite3_errmsg(sqlite) << std::endl;

	sqlite3_finalize(stmt);
}

void  DBLink::test_output(const char* map_name) {
	int rc;
	std::string sql_cmd;

	/* Execute SQL statement */
	sql_cmd = std::string().append("select count(*) from '").append(map_name).append("';");
	rc = sqlite3_exec(sqlite, sql_cmd.c_str(), DBLink::callback_is_exist, 0, &zErrMsg);
	if (rc == SQLITE_OK) {
		std::cout << "INFO：数据库中存储的区块有" << DBLink::callback_result << "个，占用"
			<< DBLink::callback_result * (sizeof(CUBE_TYPE) * BLOCK_LENGTH_X * BLOCK_LENGTH_Y * BLOCK_LENGTH_Z >> 20) << "MB" << std::endl;
		return;
	}
	else {
		std::cout << "SQL error : " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
		return;
	}

}

/// 以下为回调函数

int DBLink::callback(void* NotUsed, int argc, char** argv, char** azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
	}
	return 0;
}

int DBLink::callback_is_exist(void* NotUsed, int argc, char** argv, char** azColName) {
	DBLink::callback(NotUsed, argc, argv, azColName);
	DBLink::callback_result = std::stoi(argv[0]);
	return 0;
}


BlockMemFlow::BlockMemFlow(const char* map_name, bool over_write) {
	this->map_name = map_name;
	for (size_t i = 0; i < LOAD_SIZE; i++)
		block_load[i] = nullptr;
	for (size_t i = 0; i < CACHE_SIZE; i++)
		block_cache[i] = nullptr;
	db.generateMap(map_name, over_write);
}

BlockMemFlow::~BlockMemFlow() {
	for (size_t i = 0; i < LOAD_SIZE; i++)
		if (block_load[i]) {
			// write back
			if (block_load[i]->isDirty()) {
				if (this->searchDbBlockPos(block_pos[i]) != 0)	//exist in db
					db.updateBlock(*block_load[i], map_name.c_str(), block_pos[i]);
				else
					db.saveBlock(*block_load[i], map_name.c_str(), block_pos[i]);
			}
			delete block_load[i];
		}
	for (size_t i = 0; i < CACHE_SIZE; i++)
		if (block_cache[i]) {
			// write back
			if (block_cache[i]->isDirty()) {
				if (this->searchDbBlockPos(block_pos[i + LOAD_SIZE]) != 0)	//exist in db
					db.updateBlock(*block_cache[i], map_name.c_str(), block_pos[i + LOAD_SIZE]);
				else
					db.saveBlock(*block_cache[i], map_name.c_str(), block_pos[i + LOAD_SIZE]);
			}
			delete block_cache[i];
		}
}

std::vector<BlockPos> BlockMemFlow::testPosition(int x, int y, int z) {
	std::vector<BlockPos> adjacentVec(this->adjacentBlock(x, y, z));
	std::vector<BlockPos> notGeneratedVec;
	for (size_t i = 0; i < adjacentVec.size(); i++)
	{
		if ((this->searchMemBlockPos(adjacentVec[i]) == -1) && (this->searchDbBlockPos(adjacentVec[i]) == 0))
			notGeneratedVec.push_back(adjacentVec[i]);
	}

	return notGeneratedVec;
}

bool BlockMemFlow::loadPosition(int x, int y, int z) {
	std::vector<BlockPos> adjacentVec(this->adjacentBlock(x, y, z));
	std::vector<BlockPos> needVec;

	int exist[(BLOCK_LOAD_NUM + 1) * (BLOCK_LOAD_NUM + 1) * (BLOCK_LOAD_NUM + 1)] = { 0 };
	for (size_t i = 0; i < adjacentVec.size(); i++)
	{
		int pos = this->searchMemBlockPos(adjacentVec[i]);
		if (pos >= 0)	/// 内存中存在
			exist[pos]++;
		else
		{
			if (!this->searchDbBlockPos(adjacentVec[i])) {
				std::cout << "Error：当前区块未加载，坐标为（" << adjacentVec[i].block_x << "，" << adjacentVec[i].block_y << "，" << adjacentVec[i].block_z << "）" << std::endl;
				return false;
			}
			else	/// 从数据库调入
				needVec.push_back(adjacentVec[i]);
		}
	}

	size_t j = LOAD_SIZE, k = LOAD_SIZE, need_pos = 0;
	for (size_t i = 0; i < LOAD_SIZE; i++)
	{
		if (exist[i] == 0)
		{
			while (exist[j] == 0 && j < LOAD_SIZE + CACHE_SIZE)	/// 寻找cache中的缓存
				j++;
			if (j >= LOAD_SIZE + CACHE_SIZE)	/// 从数据库调入
			{
				//while (exist[k] != 0 && k < LOAD_SIZE + CACHE_SIZE)	/// 从cache中抛弃
				//	k++;
				k = nru_cache_search(exist + LOAD_SIZE) + LOAD_SIZE;

				Block* tmp = block_load[i];
				block_load[i] = block_cache[k - LOAD_SIZE];
				block_cache[k - LOAD_SIZE] = tmp;
				BlockPos block_tmp = block_pos[i];
				block_pos[i] = block_pos[k];
				block_pos[k] = block_tmp;

				if (block_load[i] == nullptr)
					block_load[i] = new Block;
				else if (block_load[i]->isDirty()) {
					if (this->searchDbBlockPos(block_pos[i]) != 0)	//exist
						db.updateBlock(*block_load[i], map_name.c_str(), block_pos[i]);
					else
						db.saveBlock(*block_load[i], map_name.c_str(), block_pos[i]);
				}
				db.loadBlock(*block_load[i], map_name.c_str(), needVec[need_pos]);
				block_pos[i] = needVec[need_pos];
				need_pos++;
				//k++;
			}
			else	/// 从cache调入
			{
				Block* tmp = block_load[i];
				block_load[i] = block_cache[j - LOAD_SIZE];
				block_cache[j - LOAD_SIZE] = tmp;
				BlockPos block_tmp = block_pos[i];
				block_pos[i] = block_pos[j];
				block_pos[j] = block_tmp;
				j++;
			}
		}
	}
	return true;
}

void BlockMemFlow::newPosition(Block* data, const BlockPos& block) {
#ifdef DEBUG_DBLINK
	if (this->searchMemBlockPos(block) != -1)
		std::cout << "Error：当前区块生成过位于内存，坐标为（" << block.block_x << "，" << block.block_y << "，" << block.block_z << "）" << std::endl;
	if (this->searchDbBlockPos(block) != 0)
		std::cout << "Error：当前区块生成过位于数据库，坐标为（" << block.block_x << "，" << block.block_y << "，" << block.block_z << "）" << std::endl;
#endif // DEBUG_DBLINK
	int index = nru_cache_search();	//todo test
	if (block_cache[index] != nullptr && block_cache[index]->isDirty()) {
		if (this->searchDbBlockPos(block_pos[index + LOAD_SIZE]) != 0)	//exist in db
			db.updateBlock(*block_cache[index], map_name.c_str(), block_pos[index + LOAD_SIZE]);
		else
			db.saveBlock(*block_cache[index], map_name.c_str(), block_pos[index + LOAD_SIZE]);
	}
	if (block_cache[index] != nullptr)
		delete block_cache[index];
	data->write();	//新块默认dirty
	data->setUse();	//NRU设为访问过

	block_cache[index] = data;
	block_pos[index + LOAD_SIZE] = block;
}


Block* BlockMemFlow::getPosition(const BlockPos& block) {
	int index = this->searchMemBlockPos(block);
	if (index >= 0 && index < LOAD_SIZE)
		return block_load[index];
	else
	{
		//if(index<0)
		//	std::cout << "Error：当前区块未加载，坐标为（" << block.block_x << "，" << block.block_y << "，" << block.block_z << "）" << std::endl;
		return nullptr;
	}
}

Block* BlockMemFlow::getCachePosition(const BlockPos& block) {
	int index = this->searchMemBlockPos(block);
	if (index >= LOAD_SIZE && index < CACHE_SIZE)
		return block_cache[index- LOAD_SIZE];
	else
	{
		return nullptr;
	}
}

void BlockMemFlow::test_output() {
	int active_num = 0, cache_num = 0;
	std::cout << "INFO：当前加载区块有：" << std::endl;
	for (size_t i = 0; i < LOAD_SIZE; i++)
		if (block_load[i] != nullptr)
		{
			std::cout << "（" << block_pos[i].block_x << "，" << block_pos[i].block_y << "，" << block_pos[i].block_z << "）；";
			active_num++;
		}
	std::cout << std::endl << "加载区块有" << active_num << "块，占用"
		<< active_num * (sizeof(CUBE_TYPE) * BLOCK_LENGTH_X * BLOCK_LENGTH_Y * BLOCK_LENGTH_Z >> 20) << "MB内存" << std::endl
		<< "INFO：当前缓存区块有：" << std::endl;
	for (size_t i = 0; i < CACHE_SIZE; i++)
		if (block_cache[i] != nullptr)
		{
			std::cout << "（" << block_pos[i + LOAD_SIZE].block_x << "，" << block_pos[i + LOAD_SIZE].block_y << "，" << block_pos[i + LOAD_SIZE].block_z << "）；";
			cache_num++;
		}
	std::cout << std::endl << "缓存区块有" << cache_num << "块，占用"
		<< cache_num * (sizeof(CUBE_TYPE) * BLOCK_LENGTH_X * BLOCK_LENGTH_Y * BLOCK_LENGTH_Z >> 20) << "MB内存" << std::endl;
	db.test_output(map_name.c_str());
}


std::vector<BlockPos> BlockMemFlow::adjacentBlock(int x, int y, int z) {
	BlockPos center_pos(x, y, z);
	BlockPos cur_pos(x, y, z);
	unsigned int bias_x = (((unsigned int)x + (unsigned int)0x80000000) % BLOCK_LENGTH_X) >= BLOCK_LENGTH_X / 2;
	unsigned int bias_y = (((unsigned int)y + (unsigned int)0x80000000) % BLOCK_LENGTH_Y) >= BLOCK_LENGTH_Y / 2;
	unsigned int bias_z = (((unsigned int)z + (unsigned int)0x80000000) % BLOCK_LENGTH_Z) >= BLOCK_LENGTH_Z / 2;

	std::vector<BlockPos> vec;
	cur_pos.block_x = center_pos.block_x + bias_x - BLOCK_LOAD_NUM / 2;
	for (size_t i = 0; i < BLOCK_LOAD_NUM; i++) {
		cur_pos.block_y = center_pos.block_y + bias_y - BLOCK_LOAD_NUM / 2;
		for (size_t y = 0; y < BLOCK_LOAD_NUM; y++) {
			cur_pos.block_z = center_pos.block_z + bias_z - BLOCK_LOAD_NUM / 2;
			for (size_t z = 0; z < BLOCK_LOAD_NUM; z++)
			{
				vec.push_back(cur_pos);
				cur_pos.block_z++;
			}
			cur_pos.block_y++;
		}
		cur_pos.block_x++;
	}

	return vec;
}

std::vector<BlockPos> BlockMemFlow::outerBlock(int x, int y, int z) {
	BlockPos center_pos(x, y, z);
	BlockPos cur_pos(x, y, z);
	unsigned int bias_x = (((unsigned int)x + (unsigned int)0x80000000) % BLOCK_LENGTH_X) >= BLOCK_LENGTH_X / 2;
	unsigned int bias_y = (((unsigned int)y + (unsigned int)0x80000000) % BLOCK_LENGTH_Y) >= BLOCK_LENGTH_Y / 2;
	unsigned int bias_z = (((unsigned int)z + (unsigned int)0x80000000) % BLOCK_LENGTH_Z) >= BLOCK_LENGTH_Z / 2;

	std::vector<BlockPos> vec;
	cur_pos.block_x = center_pos.block_x - BLOCK_LOAD_NUM / 2;
	for (size_t i = 0; i <= BLOCK_LOAD_NUM; i++) {
		cur_pos.block_y = center_pos.block_y - BLOCK_LOAD_NUM / 2;
		for (size_t y = 0; y <= BLOCK_LOAD_NUM; y++) {
			cur_pos.block_z = center_pos.block_z - BLOCK_LOAD_NUM / 2;
			for (size_t z = 0; z <= BLOCK_LOAD_NUM; z++)
			{
				vec.push_back(cur_pos);
				cur_pos.block_z++;
			}
			cur_pos.block_y++;
		}
		cur_pos.block_x++;
	}

	return vec;
}


int BlockMemFlow::searchMemBlockPos(const BlockPos& block) {
	for (size_t i = 0; i < LOAD_SIZE + CACHE_SIZE; i++)
	{
		if (block_pos[i] == block)
			return i;
	}

	return -1;
}

int BlockMemFlow::searchDbBlockPos(const BlockPos& block) {
	return db.isExistBlock(map_name.c_str(), block);
}

size_t BlockMemFlow::nru_cache_search(const int* exist) {
	size_t pointer_pos = nru_pointer;
	int search_stage = -1;
	bool loop = true;
	nru_pointer--;
	while (loop)
	{
		//向前走一格
		nru_pointer++;
		if (nru_pointer >= CACHE_SIZE)
			nru_pointer -= CACHE_SIZE;
		if (nru_pointer == pointer_pos)
			search_stage++;

		switch (search_stage)
		{
			case 0:
				if (block_cache[nru_pointer] == nullptr || (!block_cache[nru_pointer]->isDirty() && !block_cache[nru_pointer]->isUse()))
				{
					if (exist == nullptr || exist[nru_pointer] == 0)
						loop = false;
				}
				break;
			case 1:
				if (!block_cache[nru_pointer]->isUse(true) && block_cache[nru_pointer]->isDirty())
				{
					if (exist == nullptr || exist[nru_pointer] == 0)
						loop = false;
				}
				break;
			case 2:
				// 写在前防止被优化
				if (!block_cache[nru_pointer]->isDirty() && !block_cache[nru_pointer]->isUse())
				{
					if (exist == nullptr || exist[nru_pointer] == 0)
						loop = false;
				}
				break;
			case 3:
				if (block_cache[nru_pointer]->isDirty() && !block_cache[nru_pointer]->isUse())
				{
					if (exist == nullptr || exist[nru_pointer] == 0)
						loop = false;
				}
				break;
			default:
				std::cout << "Error：NRU算法没有找到可替换的块" << std::endl;
				loop = false;
				break;
		}
	}

	//向前走一格
	pointer_pos = nru_pointer;
	nru_pointer++;
	if (nru_pointer >= CACHE_SIZE)
		nru_pointer -= CACHE_SIZE;
	std::cout << "INFO：NRU算法替换成功" << std::endl;
	return pointer_pos;
}