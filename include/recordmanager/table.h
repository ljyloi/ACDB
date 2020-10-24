#pragma once

#include "../utils/constant.h"

//需要对齐以方便地转换为指针
struct TableHead {
  char name[MAX_NAME_LEN];
  int attr_cnt;
  int page_cnt;               //特指记录所占的页的数量，初始时为0
  int record_real_len;        //真实所占空间，字节对齐
  int record_len;             //加上冗余空间后所占空间
  int record_per_page;
  int page_index_len;         //页首部索引所占的长度
  char attr_name[MAX_ATTR_NUM][MAX_NAME_LEN];
  int attr_len[MAX_ATTR_NUM];
  int attr_type[MAX_ATTR_NUM];
  int attr_offset[MAX_ATTR_NUM];
  // 需要的字段可以继续添加
};

class Table {
  TableHead th;

};