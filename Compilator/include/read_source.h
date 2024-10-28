#ifndef READ_SOURCE
#define READ_SOURCE

#include <stdlib.h>
#include <ctype.h>

const char SEP = '\0';

enum ErrorCompilation
{
    COMP_ERROR_NO            = 0,
    COMP_ERROR_NULL_PTR      = 1,
    COMP_ERROR_ALLOCATION    = 2,
    COMP_ERROR_NO_FILE       = 3,
    COMP_ERROR_FLAG          = 4,
    COMP_FEW_ARGUMENTS       = 5,
    COMP_MANY_ARGUMENTS      = 6,
    COMP_NONE_REGISTER       = 7,
    COMP_ERROR_SYNTAX        = 8,
    COMP_EXPECTED_CONST      = 9,
    COMP_ERROR_IN_ARG        = 10,
    COMP_NONE_LABEL          = 11,
    COMP_ERROR_WRITE         = 12,
    COMP_ERROR_IN_NAME_LABEL = 13
};

struct CmpFile
{
    char *ptr_str;
    size_t size;
};

struct ArrayPtr
{
    char **ptr;
    int size;
};

struct SourceStruct
{
    ArrayPtr array_ptr;
    CmpFile file;
};

struct Files
{
    const char *comp;
    const char *run;
};

struct RunFile
{
    int *ptr;
    size_t size;
};

ErrorCompilation read_source(SourceStruct *cmp_file, const char* filename);
ErrorCompilation read_command_line(int argc, const char * const *argv, Files *files);
ErrorCompilation write_file(const char* filename, RunFile *run_file);
const char* print_error(ErrorCompilation error);

#endif
