#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

#include "run.h"

ErrorExec read_bynary(const char *filename, RunFile *runfile)
{
    struct stat st = {};
    stat(filename, &st);
    runfile->size = (st.st_size + 1) / sizeof(int);

    runfile->ptr = (int*)calloc(runfile->size, sizeof(int));
    if (runfile->ptr == NULL)
        return EXEC_ERROR_ALLOCATION;

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
        return EXEC_ERROR_NO_FILE;

    fread(runfile->ptr, sizeof(int), runfile->size, fp);

    fclose(fp);
    return EXEC_ERROR_NO;
}


