#include <recordmanager/rm.h>
#include <fileio/FileManager.h>

inline int align(int len) {
    return ((len + 3) >> 2) << 2;
}

inline int RecordPerPage(int record_len) {
    int record_per_page = PAGE_SIZE / record_len;
    while (align(record_per_page) + record_per_page * record_len > PAGE_SIZE)
        record_per_page--;
    return record_per_page;
}

RecordManager::RecordManager() {
    file_manager = FileManager::instance();
    buf_page_manager = BufPageManager::instance();
    buf_page_manager->init(file_manager);
}

int RecordManager::CreateFile(const std::string& table_name, const std::vector<Attr>& attrs) {
    //创建文件获取 id
    if (!file_manager->createFile(table_name.c_str())) 
        return -1;
    int fileId;
    if (!file_manager->openFile(table_name.c_str(), fileId)) {
        return -1;
    }
    
    name2th[table_name] = TableHead(); 

    TableHead* th = &name2th[table_name];

    // 对首页进行初始化
    strcpy(th->name, table_name.c_str());
    th->attr_cnt = attrs.size();
    th->record_real_len = 1;
    for (int i = 0; i < attrs.size(); i++) {
        th->attr_type[i] = attrs[i].type;
        strcpy(th->attr_name[i], attrs[i].name.c_str());
        th->attr_size[i] = attrs[i].byte_cnt();
        th->attr_offset[i] = th->record_real_len;
        th->record_real_len += align(th->attr_size[i]);
    }
    th->record_len = th->record_real_len * 2;
    th->record_per_page = RecordPerPage(th->record_len);
    th->page_index_len = align(th->record_per_page);
    th->page_cnt = 0;
    th->free_rid_list = 0;
    //标记修改
    int index = 0;
    auto page = buf_page_manager->allocPage(fileId, 0, index);
    memcpy(page, th, PAGE_SIZE);
    buf_page_manager->markDirty(index);
}

int RecordManager::DeleteFile(const std::string& table_name) {
    
    //TODO
}

int RecordManager::OpenFile(const std::string& table_name, Table& table) {
    if (!file_manager->openFile(table_name.c_str(), table.fileId)) {
        return -1;
    }

    int index = 0;
    auto buf = buf_page_manager->getPage(table.fileId, 0, index);  
    //打开时取出 tablehead 放在内存中操作
    memcpy(&table.th, buf, PAGE_SIZE);                              

    table.ready = true;
    //初始时 buf 无页面
    table.cur_page = -1;
    return 0;
}

int RecordManager::CloseFile(Table& table) {
    if (!table.ready)
        return 0;
    int index = 0;
    auto buf = buf_page_manager->getPage(table.fileId, 0, index);
    memcpy(buf, &table.th, PAGE_SIZE);
    buf_page_manager->markDirty(index);                             //关闭时将对应的 tablehead 写回

    table.ready = false;

    buf_page_manager->close();
    file_manager->closeFile(table.fileId);
    return 0;
}

RecordManager::~RecordManager() {
    delete file_manager;
}