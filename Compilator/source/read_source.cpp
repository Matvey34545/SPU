#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "read_source.h"

const char *EXEC_FILE = "exec.txt";

const char SEP_SYMBOL     = '\n';
const char COMMENT_SYMBOL = ';';

static ErrorCompilation read_on_file(const char* filename, CmpFile *file);
static ErrorCompilation create_array_ptr(CmpFile *file, ArrayPtr *array_ptr);
static int count_lines(const CmpFile *file, char sep, char coment_symbol);

ErrorCompilation read_source(SourceStruct *cmp_file, const char* filename)
{
    if (filename == NULL)
        return COMP_ERROR_NULL_PTR;

    ErrorCompilation error = read_on_file(filename, &cmp_file->file);
    if (error != COMP_ERROR_NO)
        return error;

    error = create_array_ptr(&cmp_file->file, &cmp_file->array_ptr);
    if (error != COMP_ERROR_NO)
        return error;

    return COMP_ERROR_NO;
}

static ErrorCompilation read_on_file(const char* filename, CmpFile *file)
{
    struct stat st = {};
    stat(filename, &st);
    file->size = st.st_size + 1;

    file->ptr_str = (char*)calloc(file->size, sizeof(char));
    if (file->ptr_str == NULL)
        return COMP_ERROR_ALLOCATION;

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
        return COMP_ERROR_NO;

    fread(file->ptr_str, sizeof(char), file->size - 1, fp);

    fclose(fp);

    *(file->ptr_str + file->size - 1) = SEP_SYMBOL;

    return COMP_ERROR_NO;
}

static ErrorCompilation create_array_ptr(CmpFile *file, ArrayPtr *array_ptr)
{
    array_ptr->size = count_lines(file, SEP_SYMBOL, COMMENT_SYMBOL);
    array_ptr->ptr = (char**)calloc(array_ptr->size, sizeof(char*));
    if (array_ptr->ptr == NULL)
        return COMP_ERROR_ALLOCATION;

    int number_symbol = 0;
    int number_not_space_symbl = 0;
    int number_string = 0;
    char *ptr_for_string = file->ptr_str;

    while (number_symbol < file->size)
    {
        if (isspace(file->ptr_str[number_symbol]) == 0)
            number_not_space_symbl++;

        if (file->ptr_str[number_symbol] == SEP_SYMBOL)
        {
            if (number_not_space_symbl > 0)
            {
                number_not_space_symbl = 0;
                array_ptr->ptr[number_string] = ptr_for_string;
                file->ptr_str[number_symbol] = SEP;
                number_string++;
            }
            ptr_for_string = file->ptr_str + number_symbol + 1;
        }

        if (file->ptr_str[number_symbol] == COMMENT_SYMBOL)
        {
            if (number_not_space_symbl > 0)
            {
                number_not_space_symbl = 0;
                array_ptr->ptr[number_string] = ptr_for_string;
                file->ptr_str[number_symbol] = SEP;
                number_string++;
            }
            while (file->ptr_str[number_symbol] != SEP_SYMBOL)
                number_symbol++;

            number_symbol++;
            ptr_for_string = file->ptr_str + number_symbol;
        }
        number_symbol++;
    }
    return COMP_ERROR_NO;
}

static int count_lines(const CmpFile *file, char sep, char coment_symbol)
{
    int number_string = 0;
    int number_not_space_symbol = 0;
    int number_symbol = 0;

    while (number_symbol < file->size)
    {
        if (isspace(file->ptr_str[number_symbol]) == 0)
            number_not_space_symbol++;

        if (file->ptr_str[number_symbol] == sep)
        {
            if (number_not_space_symbol > 0)
            {
                number_not_space_symbol = 0;
                number_string++;
            }
        }

        if (file->ptr_str[number_symbol] == coment_symbol)
        {
            if (number_not_space_symbol > 0)
            {
                number_not_space_symbol = 0;
                number_string++;
            }
            while (file->ptr_str[number_symbol] != sep)
                number_symbol++;

            number_symbol++;
        }
        number_symbol++;
    }
    printf("NUMBER STRING: %d\n", number_string);
    return number_string;
}

ErrorCompilation read_command_line(int argc, const char *const* argv, Files *files)
{
    if (argc != 4 && argc != 2)
        return COMP_ERROR_FLAG;

    files->comp = *(argv + 1);

    if (argc == 4)
    {
        if (strcmp(*(argv + 2), "-o") == 0)
            files->run = *(argv + 3);
        else
            return COMP_ERROR_FLAG;
    }
    else
        files->run = EXEC_FILE;
    return COMP_ERROR_NO;
}

ErrorCompilation write_file(const char* filename, RunFile *run_file)
{
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
        return COMP_ERROR_NO_FILE;

    int count = fwrite(run_file->ptr, sizeof(int), run_file->size, fp);
    if (count != run_file->size)
        return COMP_ERROR_WRITE;

    fclose(fp);
    return COMP_ERROR_NO;
}












