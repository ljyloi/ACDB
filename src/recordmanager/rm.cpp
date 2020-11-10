#include <recordmanager/rm.h>

/**
 * len 字节所占 4字节 的长度
 */
inline int align_len(int len) { 
    return ((len + 3) >> 2);
}


RecordManager::RecordManager() {
    file_manager = FileManager::instance();
    buf_page_manager = BufPageManager::instance();
    buf_page_manager->init(file_manager);
}

int RecordManager::CreateFile(const std::string& table_name,  std::vector<Attr>& attrs) {
    //创建文件获取 id
    if (!file_manager->createFile(table_name.c_str())) 
        return -1;
    int fileId;
    if (!file_manager->openFile(table_name.c_str(), fileId)) {
        return -1;
    }
    
    // name2th[table_name] = TableHead(); 

    // TableHead* th = &name2th[table_name];
    TableHead *th = new TableHead();

    // 对首页进行初始化
    strcpy(th->name, table_name.c_str());
    th->attr_cnt = attrs.size();
    // 记录头部留有记录 NULL 的位图
    th->record_real_len = 1;
    for (int i = 0; i < attrs.size(); i++) {
        th->attr_type[i] = attrs[i].type;
        strcpy(th->attr_name[i], attrs[i].name.c_str());
        th->attr_size[i] = attrs[i].byte_cnt();
        th->attr_offset[i] = th->record_real_len;
        th->record_real_len += align_len(th->attr_size[i]);
    }
    th->record_len = th->record_real_len * 2;
    th->record_per_page = min(MAX_RECORD_PER_PAGE, (int)(PAGE_INT_NUM - (sizeof(PageInfo) / 4)) / th->record_len);
    th->page_cnt = 0;
    th->free_rid_list = 0;
    //标记修改
    int index = 0;
    BufType page = buf_page_manager->allocPage(fileId, 0, index);
    memcpy(page, th, PAGE_SIZE);
    buf_page_manager->markDirty(index);
    buf_page_manager->close();
    return 0;
}

int RecordManager::DeleteFile(const std::string& table_name) {
    //TODO
    
}

int RecordManager::OpenFile(const std::string& table_name, Table& table) {

    if (!file_manager->openFile(table_name.c_str(), table.fileId)) {
        return -1;
    }
    int index = 0;
    printf("%d\n", table.fileId);
    BufType buf = buf_page_manager->getPage(table.fileId, 0, index);  
    //打开时取出 tablehead 放在内存中操作
    
    memcpy(&(table.th), buf, sizeof(TableHead));                              
    table.ready = true;
    //初始时 buf 无页面
    table.cur_page = -1;
    // table.th.out();
    return 0;
}

int RecordManager::CloseFile(Table& table) {
    if (!table.ready)
        return 0;
    int index = 0;
    BufType buf = buf_page_manager->getPage(table.fileId, 0, index);
    memcpy(buf, &table.th, sizeof(TableHead));
    buf_page_manager->markDirty(index);                             //关闭时将对应的 tablehead 写回

    table.ready = false;

    buf_page_manager->close();
    file_manager->closeFile(table.fileId);
    return 0;
}
