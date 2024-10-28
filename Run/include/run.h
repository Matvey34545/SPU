#ifndef RUN
#define RUN

#include <stdlib.h>

#include "stack.h"

struct Cmd
{
    unsigned int is_memory   : 1;
    unsigned int is_register : 1;
    unsigned int is_cnst     : 1;
    unsigned int command     : 5;
};

struct ArgCommand
{
    unsigned int num_reg  : 4;
    int cnst              : 28;
};

enum Command
{
    PUSH = 0,
    ADD  = 1,
    SID  = 2,
    MUL  = 3,
    DIV  = 4,
    OUT  = 5,
    IN   = 6,
    HTL  = 7,
    DUMP = 8,
    JUMP = 9,
    POP  = 10,
    JB   = 11,
    JBE  = 12,
    JA   = 13,
    JAE  = 14,
    JE   = 15,
    CALL = 16,
    RESET= 17,
    JNE  = 18,
    DRAW = 19,
    SQRT = 20
};

struct RunFile
{
    int *ptr;
    size_t size;
};

struct Processor
{
    int *num_reg;
    int size_memory;
    int *ptr_memory;
    int stck_descr;
    int stck_adr_descr;
    int return_adress;
    int ip;
};

enum ErrorExec
{
    EXEC_ERROR_NO         = 0,
    EXEC_ERROR_NO_FILE    = 1,
    EXEC_ERROR_ALLOCATION = 2,
    EXEC_STACK_ERROR      = 3,
    EXEC_END_RUN_FILE     = 4,
    EXEC_NONE_COMMAND     = 5
};


ErrorExec run_exec_file(const RunFile *runfile);
ErrorExec read_bynary(const char *filename, RunFile *runfile);



#endif
