#pragma once


enum AttrType {
  at_int, at_varchar, at_date
};

struct Attr {
  std::string name;
  AttrType type;
  int len;
};