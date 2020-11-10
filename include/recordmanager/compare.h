#pragma once
#include "attr.h"

class Comparator {
public:
    static Comparator& instance() {
        static Comparator comparator;
        return comparator;
    }
    bool CompareInt(int* a, int* b, CompareOp op) {
        switch (op)
        {
        case com_eq:
            return *a == *b;
            break;
        case com_g:
            return *a > *b;
        case com_l:
            return *a < *b;
        case com_ge:
            return *a >= *b;
        case com_le:
            return *a <= *b;
        default:
            break;
        }
        return false;
    }

    bool CompareFloat(float* a, float* b, CompareOp op) {
        switch (op)
        {
        case com_eq:
            return *a == *b;
            break;
        case com_g:
            return *a > *b;
        case com_l:
            return *a < *b;
        case com_ge:
            return *a >= *b;
        case com_le:
            return *a <= *b;
        default:
            break;
        }
        return false;
    }

    bool CompareVarchar(char* a, char* b, CompareOp op) {
        switch (op)
        {
        case com_eq:
            // printf("nicegg %s %s", a, b);
            return strcmp(a, b) == 0;
        case com_l:
            return strcmp(a, b) < 0;
        case com_g:
            return strcmp(a, b) > 0;
        case com_ge:
            return strcmp(a, b) >= 0;
        case com_le:
            return strcmp(a, b) <= 0;
        default:
            break;
        }
    }

    bool CompareData(const void* a, const void* b, CompareOp op, AttrType type) {
        switch (type)
        {
        case at_int:
            return CompareInt((int*)a, (int*)b, op);
        case at_float:
            return CompareFloat((float*)a, (float*)b, op);
        case at_varchar:
            return CompareVarchar((char*)a, (char*)b, op);
        default:
            break;
        }
        return false;
    }
};