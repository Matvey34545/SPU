#include "bynar_tree.h"
#include "debug_stack.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void dtor_tree(Top *top);
static ErrorBynarTree ver_bynar(Top *top, int (*comparison)(const void*, const void*));
static ErrorBynarTree check_on_null_ptr(const BynarTree *tree, const void *value, int (*comparison)(const void*, const void*));

ErrorBynarTree create_bynar_tree(BynarTree *tree, size_t size_element)
{
    if (tree == NULL)
        return BYNAR_TREE_NULL_PTR;

    tree->root = NULL;
    tree->size_element = size_element;

    return BYNAR_OK;
}

ErrorBynarTree insert_top(BynarTree *tree, const void *value, int (*comparison)(const void*, const void*))
{
    ErrorBynarTree error = check_on_null_ptr(tree, value, comparison);
    if (error != BYNAR_OK)
        return error;

    if (tree->root == NULL)
    {
        error = create_bynar_top(value, tree->size_element, &tree->root);
        return error;
    }

    Top *top_last = tree->root;
    Top *top_next = tree->root;
    while (top_next != NULL)
    {
        top_last = top_next;
        if (comparison(value, top_last->element) >= 0)
            top_next = top_last->right;
        else
            top_next = top_last->left;
    }

    error = create_bynar_top(value, tree->size_element, &top_next);

    if (error != BYNAR_OK)
        return error;

    if (comparison(value, top_last->element) >= 0)
        top_last->right = top_next;
    else
        top_last->left = top_next;

    return BYNAR_OK;
}

void *find_elem(const BynarTree *tree, const void *value, int (*comparison)(const void*, const void*))
{
    ErrorBynarTree error = check_on_null_ptr(tree, value, comparison);
    if (error != BYNAR_OK)
        return NULL;

    Top *top_last = tree->root;
    Top *top_next = tree->root;
    bool is_find = false;

    while (top_next != NULL && !is_find)
    {
        top_last = top_next;
        if (comparison(value, top_last->element) > 0)
            top_next = top_last->right;

        else
        {
            if (comparison(value, top_last->element) < 0)
                top_next = top_last->left;

            else
                is_find = true;
        }
    }
    if (is_find)
        return top_last->element;

    return NULL;
}

bool delete_elem(BynarTree *tree, const void *value, int (*comparison)(const void*, const void*))
{
    ErrorBynarTree error = check_on_null_ptr(tree, value, comparison);
    if (error != BYNAR_OK)
        return false;

    Top *top_last_last = tree->root;
    Top *top_last = tree->root;
    Top *top_next = tree->root;
    bool is_find = false;

    while (top_next != NULL &&  !is_find)
    {
        top_last_last = top_last;
        top_last = top_next;
        if (comparison(value, top_last->element) > 0)
            top_next = top_last->right;

        else
        {
            if (comparison(value, top_last->element) < 0)
                top_next = top_last->left;

            else
                is_find = true;
        }
    }
    if (!is_find)
        return is_find;

    Top *delete_top = top_last;

    if (delete_top->right == NULL)
    {
        if (comparison(top_last_last->element, delete_top->element) >= 0)
            top_last_last->left = delete_top->left;
        else
            top_last_last->right = delete_top->left;
    }
    else
    {
        top_next = delete_top->right;
        top_last = delete_top->right;
        while (top_next != NULL)
        {
            top_last = top_next;
            top_next = top_last->left;
        }
        top_last->left = delete_top->left;
        if (comparison(top_last_last->element, delete_top->element) >= 0)
        {
            if (comparison(top_last_last->element, delete_top->element) == 0)
                tree->root = delete_top->right;
            else
                top_last_last->left = delete_top->right;
        }
        else
            top_last_last->right = delete_top->right;
    }
    free(delete_top->element);
    free(delete_top);
    return true;
}

ErrorBynarTree destroy_bynar_tree(BynarTree *tree)
{
    if (tree == NULL)
        return BYNAR_TREE_NULL_PTR;

    if (tree->root != NULL)
    {
        dtor_tree(tree->root);
        free(tree->root);
        if (tree->root->element != NULL)
            free(tree->root->element);
    }
    return BYNAR_OK;
}

static void dtor_tree(Top *top)
{
    if (top->left != NULL)
    {
        free(top->left);
        if (top->left->element != NULL)
            free(top->left->element);

        dtor_tree(top->left);
    }
    if (top->right != NULL)
    {
        free(top->right);
        if (top->left->element != NULL)
            free(top->left->element);

        dtor_tree(top->right);
    }
}

ErrorBynarTree create_bynar_top(const void *value, size_t size_value, Top **top)
{
    if (value == NULL)
        return BYNAR_VALUE_NULL_PTR;

    if (top == NULL)
        return BYNAR_TOP_NULL_PTR;

    *top = (Top*)calloc(sizeof(Top), 1);
    if (*top == NULL)
        return BYNAR_ERROR_ALLOCATION;

    (*top)->element = calloc(1, size_value);
    if ((*top)->element == NULL)
        return BYNAR_ERROR_ALLOCATION;

    memcpy((*top)->element, value, size_value);
    return BYNAR_OK;
}

ErrorBynarTree verification_bynar_tree(const BynarTree *tree, int (*comparison)(const void*, const void*))
{
    if (tree == NULL)
        return BYNAR_TREE_NULL_PTR;

    if (comparison == NULL)
        return BYNAR_COMPARISON_NULL_PTR;

    if (tree->root != NULL)
    {
        ErrorBynarTree error = ver_bynar(tree->root, comparison);
        return error;
    }
    return BYNAR_OK;
}

static ErrorBynarTree ver_bynar(Top *top, int (*comparison)(const void*, const void*))
{
    if (top->right != NULL)
    {
        if (comparison(top->right->element, top->element) < 0)
            return BYNAR_LITTLE_RIGHT;

        ErrorBynarTree error = ver_bynar(top->right, comparison);
        if (error != BYNAR_OK)
            return error;
    }
    if (top->left != NULL)
    {
        if (comparison(top->left->element, top->element) >= 0)
            return BYNAR_BIG_LEFT;

        ErrorBynarTree error = ver_bynar(top->left, comparison);
        if (error != BYNAR_OK)
            return error;
    }
    return BYNAR_OK;
}

static ErrorBynarTree check_on_null_ptr(const BynarTree *tree, const void *value, int (*comparison)(const void*, const void*))
{
    if (value == NULL)
        return BYNAR_VALUE_NULL_PTR;

    if (comparison == NULL)
        return BYNAR_COMPARISON_NULL_PTR;

    if (tree == NULL)
        return BYNAR_TREE_NULL_PTR;

    return BYNAR_OK;
}















