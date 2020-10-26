#pragma once

#include <cstring>
#include <string>

#include "table.h"

class TableManager {
public:
    TableHead th;
    std::string current_table;
    RC openTable(const std::string& table_name) {
        if (current_table == table_name)
            return 0;
        
    }
};