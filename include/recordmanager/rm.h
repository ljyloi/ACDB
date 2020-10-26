#pragma once
#include <algorithm>
#include <cstring>
#include <map>

#include "attr.h"
#include "table.h"
#include "fileio/FileManager.h"
#include "bufmanager/BufPageManager.h"

typedef int RID_t;

struct RID {
  int page_id;
  int slot_id;

  RID(int _page_id, int _slot_id) {
    page_id = _page_id;
    slot_id = _slot_id;
  }

  RID(RID_t rid) {
    page_id = rid / PAGE_SIZE;
    slot_id = rid % PAGE_SIZE;
  }

  RID_t toRID_T() {
    return page_id * PAGE_INT_NUM + slot_id;
  }
};

class RecordManager {
public:
  RecordManager();

  int CreateFile(const std::string& table_name, const std::vector<Attr>& attrs);

  int DeleteFile(const std::string& table_name);

  int OpenFile(const std::string& table_name, Table& table);

  int CloseFile(Table & table);

  ~RecordManager();

private:
  FileManager* file_manager;
  BufPageManager* buf_page_manager;
  std::map<std::string, TableHead> name2th; //文件名到ID的转换
};
