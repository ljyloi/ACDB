#pragma once
#include <algorithm>
#include <cstring>
#include <map>

#include "attr.h"
#include "table.h"
#include "fileio/FileManager.h"
#include "bufmanager/BufPageManager.h"

class RecordManager {
public:
  RecordManager();

  int CreateFile(const std::string& table_name, const std::vector<Attr>& attrs);

  int DeleteFile(const std::string& table_name);

  int OpenFile(const std::string& table_name);

  int CloseFile(const std::string& table_name);

  ~RecordManager();

private:
  FileManager* file_manager;
  BufPageManager* buf_page_manager;
  std::map<std::string, int> name2id; //文件名到ID的转换
};
