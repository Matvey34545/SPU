#include <stdio.h>

#include "compilation.h"

int main(int argc, const char *argv[])
{
    Files files           = {};
    SourceStruct cmp_file = {};
    int run[MIN_SIZE]     = {};
    RunFile run_file      = {run, 0};

    ErrorCompilation error = read_command_line(argc, argv, &files);
    if (error != COMP_ERROR_NO)
        return error;

    error = read_source(&cmp_file, files.comp);
    if (error != COMP_ERROR_NO)
        return error;

    error = compilation(&cmp_file.array_ptr, &run_file);
    if (error != COMP_ERROR_NO)
    {
        fprintf(stderr, "Error compilation: %s\n", print_error(error));
        return error;
    }
    error = write_file(files.run, &run_file);
    return error;
}
