#pragma once
#include <algorithm>
#include <cstring>
#include <vector>
#include <map>

#include "attr.h"
#include "bufmanager/BufPageManager.h"
#include "table.h"

class Table;

struct TableHead;

class RecordManager {
public:
  RecordManager();

  int CreateFile(const std::string& table_name,  std::vector<Attr>& attrs);

  int DeleteFile(const std::string& table_name);

  int OpenFile(const std::string& table_name, Table& table);

  int CloseFile(Table & table);


private:
  FileManager* file_manager;
  BufPageManager* buf_page_manager;
  std::map<std::string, TableHead> name2th; //文件名到ID的转换
};
