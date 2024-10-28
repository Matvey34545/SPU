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
    DRAW = 19,
    SQRT = 20
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
    const Command num_command;
};

struct InfoCmd
{
    const char* name;
    const int size;
    const ExecInfoCmd exec_info;
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

const InfoCmd info_commands[] = {
                                 {"PUSH", sizeof("PUSH") - 1,   {FORMAT_ARG,   true,  PUSH}  },
                                 {"ADD", sizeof("ADD") - 1,     {WHITHOUT_ARG, false, ADD}   },
                                 {"SID", sizeof("SID") - 1,     {WHITHOUT_ARG, false, SID}   },
                                 {"MUL", sizeof("MUL") - 1,     {WHITHOUT_ARG, false, MUL}   },
                                 {"DIV", sizeof("DIV") - 1,     {WHITHOUT_ARG, false, DIV}   },
                                 {"OUT", sizeof("OUT") - 1,     {WHITHOUT_ARG, false, OUT}   },
                                 {"IN", sizeof("IN") - 1,       {WHITHOUT_ARG, false, IN }   },
                                 {"HTL", sizeof("HTL") - 1,     {WHITHOUT_ARG, false, HTL}   },
                                 {"DUMP", sizeof("DUMP") - 1,   {WHITHOUT_ARG, false, DUMP}  },
                                 {"JUMP", sizeof("JUMP") - 1,   {JUMP_ARG,     true,  JUMP}  },
                                 {"POP", sizeof("POP") - 1,     {FORMAT_ARG,   false, POP}   },
                                 {"JB", sizeof("JB") - 1,       {JUMP_ARG,     true,  JB}    },
                                 {"JBE", sizeof("JBE") - 1,     {JUMP_ARG,     true,  JBE}   },
                                 {"JA", sizeof("JA") - 1,       {JUMP_ARG,     true,  JA}    },
                                 {"JAE", sizeof("JAE") - 1,     {JUMP_ARG,     true,  JAE}   },
                                 {"JE", sizeof("JE") - 1,       {JUMP_ARG,     true,  JE}    },
                                 {"CALL", sizeof("CALL") - 1,   {JUMP_ARG,     true,  CALL}  },
                                 {"RESET", sizeof("RESET") - 1, {WHITHOUT_ARG, false, RESET} },
                                 {"JNE", sizeof("JNE") - 1,     {JUMP_ARG,     true,  JNE}   },
                                 {"DRAW", sizeof("DRAW") - 1,   {WHITHOUT_ARG, false, DRAW}  },
                                 {"SQRT", sizeof("SQRT") - 1,   {WHITHOUT_ARG, false, SQRT}  }
                                };


const RegName registers[] = {{"AX", sizeof("AX") - 1}, {"BX", sizeof("BX") - 1},
                             {"CX", sizeof("CX") - 1}, {"DX", sizeof("DX") - 1}};

ErrorCompilation compilation(ArrayPtr *array_ptr, RunFile *run_file);

#endif
