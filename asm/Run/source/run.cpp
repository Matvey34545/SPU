#include <stdio.h>
#include <string.h>

#include "run.h"
#include "stack.h"

static Command command_recognition(const RunFile *runfile, int ip);
static int* address_search(const RunFile *runfile, int *ip, Processor *processor);
static ErrorStack jump_if(int descriptor, int value, int *ip, bool (*comparison)(int, int));
static ErrorStack operation_command(int descriptor, int *ip, int (*operation)(int, int));

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

ErrorStack run_exec_file(const RunFile *runfile)
{
    const int SIZE_MEMORY = 400;
    const int NUMBER_REG  = 4;
    const int SIZEBUFFER  = 100;

    ErrorStack error = OK;

    int memory[SIZE_MEMORY] = {};
    int reg[NUMBER_REG]     = {};

    Processor processor = {reg, SIZE_MEMORY, memory};

    int descriptor = create_stack(SIZEBUFFER FOR_DEBUG(, __INIT__, "descriptor"));
    int adress = create_stack(SIZEBUFFER FOR_DEBUG(, __INIT__, "adress"));

    int ip = 0;
    int temp = 0;
    int return_address = 0;

    while (ip < runfile->size)
    {
        int *value = NULL;
        Command command = command_recognition(runfile, ip);

        switch (command)
        {

            case PUSH:
                value = address_search(runfile, &ip, &processor);
                error = push_stack(descriptor, sizeof(int), value FOR_DEBUG(, __INIT__));
                if (error != OK)
                    return error;
                break;

            case POP:
                value = address_search(runfile, &ip, &processor);
                error = pop_stack(descriptor, sizeof(int), value FOR_DEBUG(, __INIT__));
                if (error != OK)
                    return error;
                break;

            case JUMP:
                value = address_search(runfile, &ip, &processor);
                ip = *value;
                break;

            case CALL:
                return_address = ip + 2;
                error = push_stack(adress, sizeof(int), &return_address FOR_DEBUG(, __INIT__));
                if (error != OK)
                    return error;

                value = address_search(runfile, &ip, &processor);
                ip = *value;
                break;

            case JB:
                value = address_search(runfile, &ip, &processor);
                error = jump_if(descriptor, *value, &ip, jb);
                if (error != OK)
                    return error;
                break;

            case JBE:
                value = address_search(runfile, &ip, &processor);
                error = jump_if(descriptor, *value, &ip, jbe);
                if (error != OK)
                    return error;
                break;

            case JA:
                value = address_search(runfile, &ip, &processor);
                error = jump_if(descriptor, *value, &ip, ja);
                if (error != OK)
                    return error;
                break;

            case JAE:
                value = address_search(runfile, &ip, &processor);
                error = jump_if(descriptor, *value, &ip, jae);
                if (error != OK)
                    return error;
                break;

            case JE:
                value = address_search(runfile, &ip, &processor);
                error = jump_if(descriptor, *value, &ip, je);
                if (error != OK)
                    return error;
                break;

            case JNE:
                value = address_search(runfile, &ip, &processor);
                error = jump_if(descriptor, *value, &ip, jne);
                if (error != OK)
                    return error;
                break;

            case ADD:
                error = operation_command(descriptor, &ip, add_operation);
                if (error != OK)
                    return error;
                break;

            case SID:
                error = operation_command(descriptor, &ip, sid_operation);
                if (error != OK)
                    return error;
                break;

            case MUL:
                error = operation_command(descriptor, &ip, mul_operation);
                if (error != OK)
                    return error;
                break;

            case DIV:
                error = operation_command(descriptor, &ip, div_operation);
                if (error != OK)
                    return error;
                break;

            case OUT:
                error = pop_stack(descriptor, sizeof(int), &temp FOR_DEBUG(, __INIT__));
                if (error != OK)
                    return error;
                printf("Answer: %d\n", temp);
                ip++;
                break;

            case IN:
                scanf("%d", &temp);
                error = push_stack(descriptor, sizeof(int), &temp FOR_DEBUG(, __INIT__));
                if (error != OK)
                    return error;
                ip++;
                break;

            case HTL:
                return error;
                break;

            case RESET:
                error = pop_stack(adress, sizeof(int), &ip FOR_DEBUG(, __INIT__));
                 if (error != OK)
                    return error;
                break;

            default:
                return error;
                break;
        }
    }
    return OK;
}

static Command command_recognition(const RunFile *runfile, int ip)
{
    Cmd temp_command = {};
    memcpy(&temp_command, runfile->ptr + ip, sizeof(temp_command));

    return (Command) temp_command.command;
}

static int* address_search(const RunFile *runfile, int *ip, Processor *processor)
{
    int arg_value = 0;
    Cmd temp_command = {};
    ArgCommand temp_arg = {};

    static int arg_const = 0;

    memcpy(&temp_command, runfile->ptr + *ip, sizeof(Cmd));
    memcpy(&temp_arg, runfile->ptr + *ip + 1, sizeof(ArgCommand));

    *ip += 2;

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

static ErrorStack jump_if(int descriptor, int value, int *ip, bool (*comparison)(int, int))
{
    int first_elem = 0;
    int second_elem = 0;

    ErrorStack error = pop_stack(descriptor, sizeof(int), &first_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return error;

    error = pop_stack(descriptor, sizeof(int), &second_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return error;

    error = push_stack(descriptor, sizeof(int), &second_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return error;

    error = push_stack(descriptor, sizeof(int), &first_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return error;

    if (comparison(first_elem, second_elem))
        *ip = value;

    return OK;
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

static ErrorStack operation_command(int descriptor, int *ip, int (*operation)(int, int))
{
    int first_elem = 0;
    int second_elem = 0;

    *ip += 1;

    ErrorStack error = pop_stack(descriptor, sizeof(int), &first_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return error;

    error = pop_stack(descriptor, sizeof(int), &second_elem FOR_DEBUG(, __INIT__));
    if (error != OK)
        return error;

    int res = operation(first_elem, second_elem);

    error = push_stack(descriptor, sizeof(int), &res FOR_DEBUG(, __INIT__));
    return error;
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










