#include <stdio.h>
#include <string.h>
#include <math.h>

#include "run.h"
#include "stack.h"

static Command command_recognition(const RunFile *runfile, int ip);
static int* address_search(const RunFile *runfile, Processor *processor);
static ErrorExec jump_if(int descriptor, int value, int *ip, bool (*comparison)(int, int));
static ErrorExec operation_command(int descriptor, int *ip, int (*operation)(int, int));
static ErrorExec exec_command(Command command, const RunFile *runfile, Processor *processor);
static void draw(Processor *processor);
static ErrorExec operation_command_one_argument(int descriptor, int *ip, double (*math_func)(double));

static bool jb(int a, int b);
static bool jbe(int a, int b);
static bool ja(int a, int b);
static bool jae(int a, int b);
static bool je(int a, int b);
static bool jne(int a, int b);

static int add_operation(int a, int b);
static int sid_operation(int a, int b);
static int mul_operation(int a, int b);
static int div_operation(int a, int b);

const int SIZE_MEMORY = 400;

ErrorExec run_exec_file(const RunFile *runfile)
{
    const int NUMBER_REG  = 4;
    const int SIZEBUFFER  = 100;

    int memory[SIZE_MEMORY] = {};
    int reg[NUMBER_REG]     = {};

    Processor processor = {reg, SIZE_MEMORY, memory, 0, 0, -1, 0};

    processor.stck_descr = create_stack(SIZEBUFFER FOR_DEBUG(, __INIT__, "descriptor"));
    processor.stck_adr_descr = create_stack(SIZEBUFFER FOR_DEBUG(, __INIT__, "adress"));

    while (processor.ip < runfile->size)
    {
        Command command = command_recognition(runfile, processor.ip);
        ErrorExec error = exec_command(command, runfile, &processor);
        if (error != EXEC_ERROR_NO)
            return error;
    }
    return EXEC_ERROR_NO;
}

static ErrorExec exec_command(Command command, const RunFile *runfile, Processor *processor)
{
    int temp = 0;
    int *value = NULL;
    ErrorStack error_stack = OK;
    ErrorExec  error       = EXEC_ERROR_NO;

    switch (command)
    {
        case PUSH:
            value = address_search(runfile, processor);
            error_stack = push_stack(processor->stck_descr, sizeof(int), value FOR_DEBUG(, __INIT__));
            if (error_stack != OK)
                return EXEC_STACK_ERROR;
            break;

        case POP:
            value = address_search(runfile, processor);
            error_stack = pop_stack(processor->stck_descr, sizeof(int), value FOR_DEBUG(, __INIT__));
            if (error_stack != OK)
                return EXEC_STACK_ERROR;
            break;

        case JUMP:
            value = address_search(runfile, processor);
            processor->ip = *value;
            break;

        case JB:
            value = address_search(runfile, processor);
            error = jump_if(processor->stck_descr, *value, &processor->ip, jb);
            if (error != EXEC_ERROR_NO)
                return error;
            break;

        case JBE:
            value = address_search(runfile, processor);
            error = jump_if(processor->stck_descr, *value, &processor->ip, jbe);
            if (error != EXEC_ERROR_NO)
                return error;
            break;

        case JA:
            value = address_search(runfile,processor);
            error = jump_if(processor->stck_descr, *value, &processor->ip, ja);
            if (error != EXEC_ERROR_NO)
                return error;
            break;

        case JAE:
            value = address_search(runfile, processor);
            error = jump_if(processor->stck_descr, *value, &processor->ip, jae);
            if (error != EXEC_ERROR_NO)
                return error;
            break;

        case JE:
            value = address_search(runfile, processor);
            error = jump_if(processor->stck_descr, *value, &processor->ip, je);
            if (error != EXEC_ERROR_NO)
                return error;
            break;

        case JNE:
            value = address_search(runfile, processor);
            error = jump_if(processor->stck_descr, *value, &processor->ip, jne);
            if (error != EXEC_ERROR_NO)
                return error;
            break;

        case ADD:
            error = operation_command(processor->stck_descr, &processor->ip, add_operation);
            if (error != EXEC_ERROR_NO)
                return error;
            break;

        case SID:
            error = operation_command(processor->stck_descr, &processor->ip, sid_operation);
            if (error != EXEC_ERROR_NO)
                return error;
            break;

        case MUL:
            error = operation_command(processor->stck_descr, &processor->ip, mul_operation);
            if (error != EXEC_ERROR_NO)
                return error;
            break;

        case DIV:
            error = operation_command(processor->stck_descr, &processor->ip, div_operation);
            if (error != EXEC_ERROR_NO)
                return error;
            break;

        case OUT:
            error_stack = pop_stack(processor->stck_descr, sizeof(int), &temp FOR_DEBUG(, __INIT__));
            if (error_stack != OK)
                return EXEC_STACK_ERROR;
            printf("Answer: %d\n", temp);
            (processor->ip)++;
            break;

        case IN:
            scanf("%d", &temp);
            error_stack = push_stack(processor->stck_descr, sizeof(int), &temp FOR_DEBUG(, __INIT__));
            if (error_stack != OK)
                return EXEC_STACK_ERROR;
            (processor->ip)++;
            break;

        case HTL:
            return EXEC_END_RUN_FILE;
            break;

        case CALL:
            processor->return_adress = processor->ip + 2;
            error_stack = push_stack(processor->stck_adr_descr, sizeof(int), &processor->return_adress FOR_DEBUG(, __INIT__));
            if (error_stack != OK)
                return EXEC_STACK_ERROR;

            value = address_search(runfile, processor);
            processor->ip = *value;
            break;

        case RESET:
            error_stack = pop_stack(processor->stck_adr_descr, sizeof(int), &processor->ip FOR_DEBUG(, __INIT__));
            if (error_stack != OK)
                return EXEC_STACK_ERROR;
            break;

        case DRAW:
            draw(processor);
            processor->ip++;
            break;

        case SQRT:
            error = operation_command_one_argument(processor->stck_descr, &processor->ip, sqrt);
            if (error != EXEC_ERROR_NO)
                return error;
            break;

        case DUMP:
            error_stack = dump_int_stack(processor->stck_descr FOR_DEBUG(, __INIT__));
            if (error_stack != OK)
                return EXEC_STACK_ERROR;
            processor->ip++;
            break;

        default:
            return EXEC_NONE_COMMAND ;
            break;
    }
    return EXEC_ERROR_NO;
}

static Command command_recognition(const RunFile *runfile, int ip)
{
    Cmd temp_command = {};
    memcpy(&temp_command, runfile->ptr + ip, sizeof(temp_command));

    return (Command) temp_command.command;
}

static int* address_search(const RunFile *runfile, Processor *processor)
{
    int arg_value = 0;
    Cmd temp_command = {};
    ArgCommand temp_arg = {};

    static int arg_const = 0;

    memcpy(&temp_command, runfile->ptr + processor->ip, sizeof(Cmd));
    memcpy(&temp_arg, runfile->ptr + processor->ip + 1, sizeof(ArgCommand));

    processor->ip += 2;

    if (temp_command.is_cnst)
        arg_value += temp_arg.cnst;

    if (temp_command.is_register)
        arg_value += processor->num_reg[temp_arg.num_reg];

    if (temp_command.is_memory)
        return processor->ptr_memory + arg_value;

    if (temp_command.is_register)
        return processor->num_reg + temp_arg.num_reg;

    arg_const = temp_arg.cnst;

    return &arg_const;
}

static ErrorExec jump_if(int descriptor, int value, int *ip, bool (*comparison)(int, int))
{
    int first_elem = 0;
    int second_elem = 0;

    ErrorStack error = pop_stack(descriptor, sizeof(int), &first_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return EXEC_STACK_ERROR;

    error = pop_stack(descriptor, sizeof(int), &second_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return EXEC_STACK_ERROR;

    error = push_stack(descriptor, sizeof(int), &second_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return EXEC_STACK_ERROR;

    error = push_stack(descriptor, sizeof(int), &first_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return EXEC_STACK_ERROR;

    if (comparison(first_elem, second_elem))
        *ip = value;

    return EXEC_ERROR_NO;
}

static bool jb(int a, int b)
{
    if (a > b)
        return true;
    return false;
}

static bool jbe(int a, int b)
{
    if (a >= b)
        return true;
    return false;
}

static bool ja(int a, int b)
{
    if (a < b)
        return true;
    return false;
}

static bool jae(int a, int b)
{
    if (a <= b)
        return true;
    return false;
}

static bool je(int a, int b)
{
    if (a == b)
        return true;
    return false;
}

static bool jne(int a, int b)
{
    if (a != b)
        return true;
    return false;
}

static ErrorExec operation_command(int descriptor, int *ip, int (*operation)(int, int))
{
    int first_elem = 0;
    int second_elem = 0;

    *ip += 1;

    ErrorStack error = pop_stack(descriptor, sizeof(int), &first_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return EXEC_STACK_ERROR;

    error = pop_stack(descriptor, sizeof(int), &second_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return EXEC_STACK_ERROR;

    int res = operation(first_elem, second_elem);

    error = push_stack(descriptor, sizeof(int), &res FOR_DEBUG(, __INIT__));
    if (error != OK)
        return EXEC_STACK_ERROR;

    return EXEC_ERROR_NO;
}

static int add_operation(int a, int b)
{
    return a + b;
}

static int sid_operation(int a, int b)
{
    return a - b;
}

static int mul_operation(int a, int b)
{
    return a * b;
}

static int div_operation(int a, int b)
{
    return a / b;
}

static void draw(Processor *processor)
{
    const int LENGHT = 20;
    const int HEIGHT = SIZE_MEMORY / LENGHT / 2;
    const char *NORMAL_COLOR = "\x1b[0m";

    int memory_cell = 0;

    const char* colors[] = {"\e[0;30m", "\e[0;31m", "\e[0;32m", "\e[0;33m",
                            "\e[0;34m", "\e[0;35m", "\e[0;36m", "\e[0;37m"};


    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j <LENGHT; j++)
        {
            const char* color = colors[*(processor->ptr_memory + memory_cell) & 0x7];
            char  symbol = (char)*(processor->ptr_memory + memory_cell + 1);
            printf("%s%c", color, symbol);
            memory_cell += 2;
        }
        putchar('\n');
    }
    printf("%s", NORMAL_COLOR);
}

static ErrorExec operation_command_one_argument(int descriptor, int *ip, double (*math_func)(double))
{
    int pop_elem = 0;

    *ip += 1;

    ErrorStack error = pop_stack(descriptor, sizeof(int), &pop_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return EXEC_STACK_ERROR;

    double temp = (double)pop_elem;

    temp = round(math_func(temp));

    int push_elem = (int)temp;

    error = push_stack(descriptor, sizeof(int), &push_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return EXEC_STACK_ERROR;

    return EXEC_ERROR_NO;
}









