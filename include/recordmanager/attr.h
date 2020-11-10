#pragma once

#include <cstring>
#include <string>

enum CompareOp {
  com_eq, com_g, com_l, com_ge, com_le
};

enum AttrType {
  at_int, at_varchar, at_date, at_float
};

struct Attr {
  std::string name;
  AttrType type;
  int len;

  Attr(const std::string& _name, const AttrType &_type, const int& _len = 1):name(_name), type(_type), len(_len){}

  bool operator ==(const Attr& b) {
    return b.name == name && b.type == type && b.len == len;
  }

  bool equal(const std::string& _name, const AttrType& _type, const int& _len) {
    return name == _name && _type == type && _len == len;
  }

  int byte_cnt() const {
    switch (type)
    {
    case at_int:
    case at_float:
      return 4;
    case at_varchar:
      return len + 1;
    }
    return 0;
  }
};


