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
    file_manager = new FileManager();
    buf_page_manager = new BufPageManager(file_manager);
}

int RecordManager::CreateFile(const std::string& table_name, const std::vector<Attr>& attrs) {
    //创建文件获取 id
    if (!file_manager->createFile(table_name.c_str())) 
        return -1;
    int fileId;
    if (!file_manager->openFile(table_name.c_str(), fileId)) {
        return -1;
    }
    name2id[table_name] = fileId;

    //分配记录首页
    int index = 0;
    TableHead* th = (TableHead*)buf_page_manager->allocPage(fileId, 0, index);

    // 对首页进行初始化
    strcpy(th->name, table_name.c_str());
    th->attr_cnt = attrs.size();
    th->record_real_len = 0;
    for (int i = 0; i < attrs.size(); i++) {
        th->attr_type[i] = attrs[i].type;
        strcpy(th->attr_name[i], attrs[i].name.c_str());
        th->attr_len[i] = attrs[i].len;
        th->record_real_len += align(attrs[i].len);
        th->attr_offset[i] = th->record_real_len;
    }
    th->record_len = th->record_real_len * 2;
    th->record_per_page = RecordPerPage(th->record_len);
    th->page_index_len = align(th->record_per_page);
    th->page_cnt = 0;

    //标记修改
    buf_page_manager->markDirty(index);
}

int RecordManager::DeleteFile(const std::string& table_name) {
    CloseFile(table_name);
    //TODO
}

int RecordManager::OpenFile(const std::string& table_name) {
    auto it = name2id.find(table_name);
    if (it != name2id.end()) 
        return 0;
    int fileId;
    if (!file_manager->openFile(table_name.c_str(), fileId)) {
        return -1;
    }
    name2id[table_name] = fileId;
    return 0;
}

int RecordManager::CloseFile(const std::string& table_name) {
    auto it = name2id.find(table_name);
    if (it == name2id.end()) 
        return -1;
    file_manager->closeFile(it->second);
    name2id.erase(table_name);
    return 0;
}

RecordManager::~RecordManager() {
    delete file_manager;
}