#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>

#include <recordmanager/rm.h>


using namespace std;
int main(int argc, char** argv) {
  if (argc == 1) {
    printf("c: create a file\no: open a file\n");
    return 0;
  }
  MyBitMap::initConst();
  RecordManager rm_manager;
  std::vector<Attr> attrs;
  attrs.push_back(Attr("name", at_varchar, 250));
  attrs.push_back(Attr("age", at_int));
  attrs.push_back(Attr("score", at_float));

  if (strcmp(argv[1], "c") == 0) {
    printf("create a file\n");
    int ret = rm_manager.CreateFile("test", attrs);
    printf("ret = %d\n", ret);
    return 0;
  }
  if (strcmp(argv[1], "o") == 0) {
    printf("open a file\n");
    Table table;
    rm_manager.OpenFile("test", table);
    table.th.out();
    rm_manager.CloseFile(table);
    return 0;
  }
  if (strcmp(argv[1], "i") == 0) {
    printf("insert a record\n");
    Table table;
    rm_manager.OpenFile("test", table);
    table.th.out();
    std::string name = "徐AC";
    float score = 100;
    int age = 20;
    std::vector<int> attrids = {0,1,2};
    std::vector<const void*> datas = {name.c_str(), &age, &score};
    table.InsertRecord(attrids, datas);
    rm_manager.CloseFile(table);
    return 0;
  }
  if (strcmp(argv[1], "q") == 0) {
    Table table;
    rm_manager.OpenFile("test", table);
    table.th.out();
    std::string name = "徐AC";
    std::vector<RID_t> get_rids;
    std::vector<int> attr_ids = {0};
    std::vector<CompareOp> ops = {com_eq};
    std::vector<const void*> datas = {name.c_str()};
    table.QueryRecord(get_rids, attr_ids, ops, datas);
    printf("result rids:");
    for (int i = 0; i < get_rids.size(); i++)
      printf("%d ", get_rids[i]);
    printf("\n");
    return 0;
  }
  if (strcmp(argv[1], "d") == 0) {
    Table table;
    rm_manager.OpenFile("test", table);
    table.th.out();
    std::string name = "徐AC";
    std::vector<RID_t> get_rids;
    std::vector<int> attr_ids = {0};
    std::vector<CompareOp> ops = {com_eq};
    std::vector<const void*> datas = {name.c_str()};
    table.QueryRecord(get_rids, attr_ids, ops, datas);
    if (get_rids.size() != 0) {
      int id = std::rand() % get_rids.size();

      printf("%d is going to be deleted\n", get_rids[id]);
      table.RemoveRecord(RID(get_rids[id]));
    }
    rm_manager.CloseFile(table);
    return 0;
  }
  return 0;
}