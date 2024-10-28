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

    for (int i = 0; i < cmp_file.array_ptr.size; i++)
        printf("%s\n", *(cmp_file.array_ptr.ptr + i));

    error = compilation(&cmp_file.array_ptr, &run_file);
    if (error != COMP_ERROR_NO)
    {
        fprintf(stderr, "Error compilation: %d\n", error);
        return error;
    }
    int ip = 0;
    while (ip < run_file.size)
    {
        ArgCommand *arg;
        Cmd *cmd = (Cmd*)run_file.ptr + ip;
        printf("Command: %d\n", cmd->command);

        switch (cmd->command)
        {
            case PUSH:
            case POP:
            case JUMP:
            case JE:
            case JNE:
            case JBE:
            case JAE:
            case JB:
            case JA:
            case CALL:
                arg = (ArgCommand*)(Cmd*)run_file.ptr + ip + 1;
                printf("memory: %d\nregister: %d\nconst: %d\n", cmd->is_memory, cmd->is_register, cmd->is_cnst);
                printf("value const: %d\nnum_reg: %d\n\n", arg->cnst, arg->num_reg);
                ip += 2;
                break;
            default:
                printf("memory: %d\nregister: %d\nconst: %d\n\n", cmd->is_memory, cmd->is_register, cmd->is_cnst);
                ip += 1;
                break;
        }

    }

    error = write_file(files.run, &run_file);
    return error;
}
