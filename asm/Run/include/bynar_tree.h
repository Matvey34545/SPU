#ifndef BYNAR_TREE
#define BYNAR_TREE

#include <stdlib.h>

enum ErrorBynarTree
{
    BYNAR_OK                  = 0,
    BYNAR_ERROR_ALLOCATION    = 1,
    BYNAR_TREE_NULL_PTR       = 2,
    BYNAR_VALUE_NULL_PTR      = 3,
    BYNAR_BIG_LEFT            = 4,
    BYNAR_LITTLE_RIGHT        = 5,
    BYNAR_COMPARISON_NULL_PTR = 6,
    BYNAR_TOP_NULL_PTR        = 7
};

struct Top // Node
{
    Top* left;
    Top* right;
    void* element;
};

struct BynarTree
{
    Top* root;
    size_t size_element;
};

ErrorBynarTree create_bynar_tree(BynarTree *tree, size_t size_element);
ErrorBynarTree insert_top(BynarTree *tree, const void *value, int (*comparison)(const void*, const void*));
void* find_elem(const BynarTree *tree, const void *value, int (*comparison)(const void*, const void*));
bool delete_elem(BynarTree *tree, const void *value, int (*comparison)(const void*, const void*));
ErrorBynarTree destroy_bynar_tree(BynarTree *tree);
ErrorBynarTree create_bynar_top(const void *value, size_t size_value, Top **top);
ErrorBynarTree verification_bynar_tree(const BynarTree *tree, int (*comparison)(const void*, const void*));

#endif
