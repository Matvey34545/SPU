#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "read_source.h"

const char *EXEC_FILE = "exec.txt";

static ErrorCompilation read_on_file(const char* filename, CmpFile *file);
static ErrorCompilation create_array_ptr(CmpFile *file, ArrayPtr *array_ptr);
static int count_lines(const CmpFile *file, char sep);

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
    assert(filename != NULL);

    struct stat st = {};
    stat(filename, &st);
    file->size = st.st_size;

    file->ptr_str = (char*)calloc(file->size, sizeof(char));
    if (file->ptr_str == NULL)
        return COMP_ERROR_ALLOCATION;

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
        return COMP_ERROR_NO;

    size_t ret_fread = fread(file->ptr_str, sizeof(char), file->size, fp);

    fclose(fp);

    return COMP_ERROR_NO;
}

static ErrorCompilation create_array_ptr(CmpFile *file, ArrayPtr *array_ptr)
{
    assert(file != NULL);
    assert(array_ptr != NULL);

    char sep = ';';

    array_ptr->size = count_lines(file, sep);
    array_ptr->ptr = (String*)calloc(array_ptr->size, sizeof(String));
    if (array_ptr->ptr == NULL)
        return COMP_ERROR_ALLOCATION;

    String *begin_array_ptr = array_ptr->ptr;
    begin_array_ptr->str = file->ptr_str;
    int number_str = 0;
    int element_str = 0;

    for (int i = 0; i < file->size; i++)
    {
        element_str++;
        if (*(file->ptr_str + i) == sep)
        {
            (begin_array_ptr + number_str + 1)->str = file->ptr_str + i + 1;
            *(file->ptr_str + i) = '\0';
            (begin_array_ptr + number_str)->len = element_str;
            element_str = 0;
            number_str++;
        }
    }
    return COMP_ERROR_NO;
}

static int count_lines(const CmpFile *file, char sep)
{
    int count = 0;
    for (int i = 0; i < file->size; i++)
    {
        if (*(file->ptr_str + i) == sep)
            count++;
    }
    return count;
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












