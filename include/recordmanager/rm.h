#pragma once
#include <algorithm>
#include <cstring>

#include "attr.h"

class RecordManager {
  int CreateFile(std::string table_name, std::vector<Attr> attrs);
  int DeleteFile(std::string table_name);
  int OpenFile(std::string table_name);
  int CloseFile(std::string table_name);
};
