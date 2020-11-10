#pragma once

#include <vector>
#include "../utils/common.h"
#include "compare.h"

typedef int RID_t;

struct RID {
  int page_id;
  int slot_id;

  RID(int _page_id, int _slot_id) {
    page_id = _page_id;
    slot_id = _slot_id;
  }

  RID(RID_t rid) {
    page_id = rid / PAGE_INT_NUM;
    slot_id = rid % PAGE_INT_NUM;
  }

  RID_t toRID_T() const {
    return page_id * PAGE_INT_NUM + slot_id;
  }

  void print() {
    printf("%d %d\n", page_id, slot_id);
  }
};


//需要对齐以方便地转换为指针
struct TableHead {
  char name[MAX_NAME_LEN];
  int attr_cnt;
  int page_cnt;               //特指记录所占的页的数量，初始时为0
  int record_real_len;        //真实所占的 4 字节数，即值为 1 时 占四个字节
  int record_len;             //加上冗余空间后所占空间
  int record_per_page;
  char attr_name[MAX_ATTR_NUM][MAX_NAME_LEN];
  int attr_size[MAX_ATTR_NUM];
  int attr_type[MAX_ATTR_NUM];
  int attr_offset[MAX_ATTR_NUM]; // 4 字节下的偏移量
  int free_rid_list;          //被释放的空闲的 slot 的 rid，构成一条链，指向的 rid 中保存下一个空闲的 rid
  // 需要的字段可以继续添加
  void out() {
    printf("table info \n");
    printf("name: %s\n", name);
    printf("page_cnt:%d\n", page_cnt);
    printf("record_len: %d record_real_len: %d\n", record_len, record_real_len);
    printf("record_per_page: %d\n", record_per_page);
    printf("free_rid_list: %d\n", free_rid_list);
    // printf("record_per_page: %d\n", record_per_page);
    for (int i = 0; i < attr_cnt; i++) {
      printf("  %s: %d %d %d\n", attr_name[i], attr_size[i], attr_type[i], attr_offset[i]);
    }
  }
};

//每一页留出空间标识这是类型的页面，并记录相关信息
struct PageInfo {
  int symbol;
  int index;

  PageInfo() {
    symbol = index = 0;
  }
}; 

// record = attr_null_bit_map(4B) + {attr}*n

class Table {
public:
  TableHead th;
  int fileId;
  bool ready;

  BufType buf;
  //Table 中当前buf中放置的页id
  int cur_page;
  PageInfo* cur_page_info;

  Table() {
    cur_page = 0;
    ready = false;
    buf = NULL;
    cur_page_info = NULL;
  }

  int getAttrId(const std::string& name) {
    for (int i = 0; i < th.attr_cnt; i++)
      if (name == th.attr_name[i])
        return i;
  }

  void switch_page(int pageId) {
    int index;
    if (cur_page == pageId)
      return;
    buf = BufPageManager::instance()->getPage(fileId, pageId, index);
    cur_page_info = (PageInfo*)(buf + PAGE_INT_NUM - sizeof(PageInfo)/4);
    cur_page = pageId;
  }
  
  void dirty_current_page() {
    BufPageManager::instance()->markDirty(cur_page);
  }

  void alloc_page() {
    th.page_cnt++;
    //将新分配的页换入 table 中便于操作，需要注意，新分配的页不一定是全0
    switch_page(th.page_cnt);
    memset(buf, 0, sizeof(buf));
    for (int i = 0; i < th.record_per_page - 1; i++) {
      buf[i * th.record_len] = RID(th.page_cnt, i + 1).toRID_T();
    }
    PageInfo page_info;
    memcpy(buf + PAGE_INT_NUM - sizeof(PageInfo)/4, &page_info, sizeof(PageInfo));
    th.free_rid_list = RID(th.page_cnt, 0).toRID_T();
  }

  RID find_slot() {
    if (!th.free_rid_list) {
      alloc_page();
    }
    return RID(th.free_rid_list);
  }

  RC InsertRecord(std::vector<int>& attrids, std::vector<const void*>& datas) {
    RID rid = find_slot();
    switch_page(rid.page_id);
    // 添加页首标记
    cur_page_info->index |= (1 << rid.slot_id);
    // 维护 free_rid_list
    int slot_pos = rid.slot_id * th.record_len;
    th.free_rid_list = buf[slot_pos];
    // 写入数据
    buf[slot_pos] = 0;
    for(int i = 0; i < attrids.size(); ++i) {
      char* p = (char*)datas[i];
      buf[slot_pos] |= 1 << attrids[i];
      memcpy(buf + slot_pos + th.attr_offset[attrids[i]], p, th.attr_size[attrids[i]]);
    }

    dirty_current_page();
  }

  RC RemoveRecord(const RID& rid) {
    switch_page(rid.page_id);
    // 去除页首标记
    int slot_pos = th.record_len * rid.slot_id;
    cur_page_info->index -= (1 << rid.slot_id);
    buf[slot_pos] = th.free_rid_list;
    th.free_rid_list = rid.toRID_T();

    dirty_current_page();
  }

  RC UpdateRecord(const RID& rid, std::vector<int>& attrids, std::vector<void*>& datas) {
    switch_page(rid.page_id);

    int slot_pos = rid.slot_id * th.record_len;
    // 写入数据
    for(int i = 0; i < attrids.size(); ++i) {
      buf[slot_pos] |= 1 << attrids[i];
      memcpy(buf + slot_pos + th.attr_offset[attrids[i]], datas[i], th.attr_size[attrids[i]]);
    }

    dirty_current_page();
  }

  bool goNext(RID& rid) {
    rid.slot_id++;
    while (1) {
      if (rid.page_id == 0 || rid.slot_id == th.record_per_page) {
        rid.page_id++;
        rid.slot_id = 0;
        continue;
      }
      if (rid.page_id > th.page_cnt) 
        return false;
      switch_page(rid.page_id);
      for (; rid.slot_id < th.record_per_page; rid.slot_id++)
        if (cur_page_info->index & (1 << rid.slot_id))  {
          // rid.print();
          return true;
        }
    }  
  }

  BufType getDataPtr(const RID& rid, int attr_id) {
    switch_page(rid.page_id);
    return buf + th.record_len * rid.slot_id + th.attr_offset[attr_id];
  }

  bool NotNull(const RID& rid, int attr_id) {
    switch_page(rid.page_id);
    return *(buf + th.record_len * rid.slot_id) & (1 << attr_id);
  }

  RC QueryRecord(std::vector<RID_t>& rids, 
                 std::vector<int> &attr_ids, 
                 std::vector<CompareOp>& ops, 
                 std::vector<const void*>& datas) {
    RID rid(0, 0);
    while (goNext(rid)) {
      // printf("working rid: ");
      // rid.print();
      bool meet = true;
      switch_page(rid.page_id);

      for (int i = 0; i < attr_ids.size(); i++) {
        if (!NotNull(rid, attr_ids[i])) {
          meet = false;
          break;
        }
        if (!Comparator::instance().CompareData(
            (void*)getDataPtr(rid, attr_ids[i]), datas[i], ops[i], (AttrType)th.attr_type[attr_ids[i]])) {
          meet = false;
          break;
        }
            
      }
      
      if (meet) {
        rids.push_back(rid.toRID_T());
      }
    }
    return 0;
  }

  //TODO
};