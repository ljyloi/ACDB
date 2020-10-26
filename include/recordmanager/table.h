#pragma once

#include "../utils/common.h"
#include "../utils/pagedef.h"
#include "attr.h"

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
  int attr_size[MAX_ATTR_NUM];
  int attr_type[MAX_ATTR_NUM];
  int attr_offset[MAX_ATTR_NUM];
  int free_rid_list;          //被释放的空闲的 slot 的 rid，构成一条链，指向的 rid 中保存下一个空闲的 rid
  // 需要的字段可以继续添加
};

// record = attr_null_bit_map(4B) + {attr}*n

class Table {
public:
  TableHead th;
  int fileId;
  bool ready;

  int buf[PAGE_INT_NUM];
  //Table 中当前buf中放置的页id
  int cur_page;

  int getAttrId(const std::string& name) {
    for (int i = 0; i < th.attr_cnt; i++)
      if (name == th.attr_name[i])
        return i;
  }

  void switch_page(int pageId) {
    if (cur_page != -1) {                                                       
      //换出原来的
      int index;
      auto page = BufPageManager::instance()->getPage(fileId, cur_page, index);
      memcpy(page, buf, PAGE_SIZE);
      BufPageManager::instance()->markDirty(index);
    }
    //换入新的
    if (cur_page == pageId)
      return;

    auto page = BufPageManager::instance()->getPage(fileId, pageId, index);
    memcpy(buf, page, PAGE_SIZE);
    cur_page = pageId;
  }

  void alloc_page() {
    th.page_cnt++;
    //将新分配的页换入 table 中便于操作，需要注意，新分配的页不一定是全0
    switch_page(th.page_cnt);
    memset(buf, 0, sizeof(buf));
    for (int i = 0; i < th.record_per_page - 1; i++) {
      buf[th.page_index_len + i * th.record_len] = RID(th.page_cnt, th.page_index_len + (i + 1) * th.record_len).toRID_T();
    }
    th.free_rid_list = RID(th.page_cnt, th.page_index_len).toRID_T();
  }

  RID find_slot() {
    if (!th.free_rid_list) {
      alloc_page();
    }
    return RID(th.free_rid_list);
  }

  RC InsertRecord(std::vector<int>& attrids, std::vector<void*>& datas) {
    RID rid = find_slot();
    printf("%d %d\n", rid.page_id, rid.slot_id);
    switch_page(rid.page_id);
    //TODO

  }

  RC RemoveRecord(const RID& rid) {
    //TODO
  }

  /TODO
};