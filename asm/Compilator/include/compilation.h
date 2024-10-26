#ifndef COMPILATION
#define COMPILATION

#include "read_source.h"

const int MIN_SIZE = 1000;

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
    LABEL= 19,
};

struct Label
{
    int number_str;
    int size;
    const char *name;
};

struct ArrLabels
{
    Label *ptr;
    size_t size;
};

struct RegName
{
    const char* name;
    const int size;
};

enum TypeArg
{
    WHITHOUT_ARG = 0,
    FORMAT_ARG   = 1,
    JUMP_ARG     = 2
};

struct ExecInfoCmd
{
    const TypeArg type_arg;
    const bool is_value;
    Command num_command;
};

struct InfoCmd
{
    const char* name;
    const int size;
    ExecInfoCmd exec_info;
};

enum NumberRegister
{
    AX            = 0,
    BX            = 1,
    CX            = 2,
    DX            = 3
};

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

ErrorCompilation compilation(ArrayPtr *array_ptr, RunFile *run_file);

#endif
