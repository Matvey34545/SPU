#include <string.h>
#include <stdio.h>

// Хочу добавить новую инструкцию - надо изменить ОДНО МЕСТО! (а не 3)
#include "compilation.h"

static Command comparing_commands(char *src, char **endptr);
static ErrorCompilation check_correct(const char *source, char sep);
static ErrorCompilation is_register(char *source, NumberRegister *num_reg, char **endptr);
static ErrorCompilation processing_argument_string(char* ptr_str, TypeArg *arg);
static ErrorCompilation first_comp(ArrLabels *arr_labels, ArrayPtr *array_ptr);
static ErrorCompilation add_label(char *str, ArrLabels *arr_labels, int ip);
static ErrorCompilation pop_command(char *str, RunFile *run_code);
static ErrorCompilation write_arg(char *str, TypeArg *arg, Cmd *cmd, ArgCommand *arg_command, Command COMMAND);
static ErrorCompilation push_command(char *str, RunFile *run_code);
static ErrorCompilation jump_command(char *str, RunFile *run_code, const ArrLabels *arr_labels, Command command);
static ErrorCompilation without_arg_command(char *str, RunFile *run_code, Command command);
static ErrorCompilation second_comp(ArrLabels *arr_labels, ArrayPtr *array_ptr, RunFile *run_file);
static ErrorCompilation create_label(char *str, Label *label, char sep);
static Label* find_label(char *str, const ArrLabels *arr_labels);
static Label* search_label(const ArrLabels *arr_labels, Label *label);
static ErrorCompilation call_command(char *str, RunFile *run_code, const ArrLabels *arr_labels);

// ArrayPtr -> ????
// Массив строк, указатели на строки
ErrorCompilation compilation(ArrayPtr *array_ptr, RunFile *run_file)
{
    Label labels[MIN_SIZE] = {};
    ArrLabels arr_labels = { .ptr = labels,
                             .size = 0};

    ErrorCompilation error = first_comp(&arr_labels, array_ptr);
    if (error != COMP_ERROR_NO)
        return error;

    error = second_comp(&arr_labels, array_ptr, run_file);
    return error;
}

// Хочу видеть функции в порядке их важности

static Command comparing_commands(char *src, char ** endptr)
{
    DefCom commands[] = {{"PUSH", sizeof("PUSH") - 1}, {"ADD", sizeof("ADD") - 1},
                         {"SID", sizeof("SID") - 1},   {"MUL", sizeof("MUL") - 1},
                         {"DIV", sizeof("DIV") - 1},   {"OUT", sizeof("OUT") - 1},
                         {"IN", sizeof("IN") - 1},     {"HTL", sizeof("HTL") - 1},
                         {"DUMP", sizeof("DUMP") - 1}, {"JUMP", sizeof("JUMP") - 1},
                         {"POP", sizeof("POP") - 1},    {"JB", sizeof("JB") - 1},
                         {"JBE", sizeof("JBE") - 1},   {"JA", sizeof("JA") - 1},
                         {"JAE", sizeof("JAE") - 1},   {"JE", sizeof("JE") - 1},
                         {"CALL", sizeof("CALL") - 1}, {"RESET", sizeof("RESET") - 1},
                         {"JNE", sizeof("JNE") - 1}
                        };


    while (isspace(*src) != 0)
        src++;

    size_t number_command = sizeof(commands) / sizeof(DefCom);
    int command = 0;

    while (command < number_command)
    {
        if (strncmp(src, commands[command].name, commands[command].size) == 0)
        {
            if (isspace(*(src + commands[command].size)) != 0 || *(src + commands[command].size) == '\0')
            {
                *endptr = src + commands[command].size;
                return Command(command);
            }
        }
        command++;
    }
    return LABEL;
}

static ErrorCompilation check_correct(const char *source, char sep)
{
    while (isspace(*source) != 0)
        source++;

    if (*source != sep)
        return COMP_MANY_ARGUMENTS;

    return COMP_ERROR_NO;
}

ErrorCompilation processing_argument_string(char* ptr_str, TypeArg *arg)
{
    const char RAM_SYMBL      = '[';
    const char BACK_RAM_SYMBL = ']';
    const char PLUS_SYMBL     = '+';
    ErrorCompilation error = COMP_ERROR_NO;

    arg->is_memory = false;
    arg->is_register = false;
    arg->is_cnst = false;

    while(isspace(*ptr_str) != 0)
        ptr_str++;   // в функцию

    int endptr = 0;

    if (*ptr_str == RAM_SYMBL)
    {
        ptr_str++;
        arg->is_memory = true;

        int is_cnst = sscanf(ptr_str, "%d%n", &arg->cnst, &endptr);
        ptr_str += endptr;

        if (is_cnst >= 1)
            arg->is_cnst = true;

        else
        {
            error = is_register(ptr_str, &arg->num_reg, &ptr_str);
            if (error != COMP_ERROR_NO)
                return error;

            arg->is_register = true;

            while(isspace(*ptr_str) != 0)
                ptr_str++;

            if (*ptr_str == PLUS_SYMBL)
            {
                ptr_str++;
                is_cnst = sscanf(ptr_str, "%d%n", &arg->cnst, &endptr);
                ptr_str += endptr;

                if (is_cnst >= 1)
                    arg->is_cnst = true;

                else
                    return COMP_EXPECTED_CONST;
            }
        }
        while(isspace(*ptr_str) != 0)
                ptr_str++;

        if (*ptr_str++ != BACK_RAM_SYMBL)
            return COMP_ERROR_SYNTAX;
    }
    else
    {
        int is_cnst = sscanf(ptr_str, "%d%n", &arg->cnst, &endptr);
        ptr_str += endptr;

        if (is_cnst >= 1)
            arg->is_cnst = true;

        else
        {
            arg->is_cnst = false;
            error = is_register(ptr_str, &arg->num_reg, &ptr_str);
            if (error != COMP_ERROR_NO)
                return error;

            arg->is_register = true;
        }
    }
    error = check_correct(ptr_str, '\0');
    return error;
}

static ErrorCompilation is_register(char *source, NumberRegister *num_reg, char **endptr)
{
    while (isspace(*source) != 0)
        source++;

    DefCom registers[] = {{"AX", sizeof("AX") - 1}, {"BX", sizeof("BX") - 1},
                          {"CX", sizeof("CX") - 1}, {"DX", sizeof("DX") - 1}};


    int number_register = sizeof(registers) / sizeof(DefCom);
    int reg = 0;

    while (reg < number_register)
    {
        if (strncmp(source, registers[reg].name, registers[reg].size) == 0)
        {
            *endptr = source + registers[reg].size;
            *num_reg = (NumberRegister) reg;
            return COMP_ERROR_NO;
        }
        reg++;
    }
    return COMP_NONE_REGISTER;
}
                                                                                                // не кричи!!!
static ErrorCompilation write_arg(char *str, TypeArg *arg, Cmd *cmd, ArgCommand *arg_command, Command COMMAND)
{
    ErrorCompilation error = processing_argument_string(str, arg);
    if (error != COMP_ERROR_NO)
        return error;

    if (arg->is_memory)
        cmd->is_memory = 1;

    if (arg->is_register)
    {
        arg_command->num_reg = (unsigned int)arg->num_reg;
        cmd->is_register = 1;
    }

    if (arg->is_cnst)
    {
        arg_command->cnst = arg->cnst;
        cmd->is_cnst = 1;
    }

    cmd->command = (unsigned int)COMMAND;
    return COMP_ERROR_NO;
}

static ErrorCompilation push_command(char *str, RunFile *run_code)
{
    // Add arg_temp (or fix НАЗВАНИЯ!!!)
    TypeArg arg = {};
    Cmd temp = {};
    ArgCommand temp_arg = {};

    ErrorCompilation error = write_arg(str, &arg, &temp, &temp_arg, PUSH);
    if (error != COMP_ERROR_NO)
        return error;

    if ( !(arg.is_register || arg.is_cnst) )
        return COMP_ERROR_IN_ARG;

    memcpy(run_code->ptr + run_code->size    , &temp    , sizeof(Cmd));
    memcpy(run_code->ptr + run_code->size + 1, &temp_arg, sizeof(ArgCommand));
    run_code->size += 2;
    return COMP_ERROR_NO;
}

static ErrorCompilation pop_command(char *str, RunFile *run_code)
{
    TypeArg arg = {};
    Cmd temp = {};
    ArgCommand temp_arg = {};

    ErrorCompilation error = write_arg(str, &arg, &temp, &temp_arg, POP);
    if (error != COMP_ERROR_NO)
        return error;

    if (!arg.is_memory)
    {
        if (arg.is_cnst)
            return COMP_ERROR_IN_ARG;
    }

    memcpy(run_code->ptr + run_code->size, &temp, sizeof(Cmd));
    memcpy(run_code->ptr + run_code->size + 1, &temp_arg, sizeof(ArgCommand));
    run_code->size += 2;
    return COMP_ERROR_NO;
}

static ErrorCompilation jump_command(char *str, RunFile *run_code, const ArrLabels *arr_labels, Command command)
{
    TypeArg arg = {};
    Cmd temp = {};
    ArgCommand temp_arg = {};

    Label* label = find_label(str, arr_labels);

    if (label != NULL)
    {
        temp_arg.cnst = label->number_str;
        temp.is_cnst = 1;
        temp.command = command;
        printf("FIND LABEL\n\n");
    }
    else
    {
        ErrorCompilation error = write_arg(str, &arg, &temp, &temp_arg, command);
        if (error != COMP_ERROR_NO)
            return error;

        if (!arg.is_memory)
        {
            if (arg.is_cnst)
                return COMP_ERROR_IN_ARG;
        }
    }

    memcpy(run_code->ptr + run_code->size, &temp, sizeof(Cmd));
    memcpy(run_code->ptr + run_code->size + 1, &temp_arg, sizeof(ArgCommand));
    run_code->size += 2;
    return COMP_ERROR_NO;
}

static ErrorCompilation call_command(char *str, RunFile *run_code, const ArrLabels *arr_labels)
{
    Cmd temp = {};
    ArgCommand temp_arg = {};

    Label* label = find_label(str, arr_labels);

    if (label != NULL)
    {
        temp_arg.cnst = label->number_str;
        temp.is_cnst = 1;
        temp.command = CALL;
    }
    else
        return COMP_NONE_LABEL;

    memcpy(run_code->ptr + run_code->size, &temp, sizeof(Cmd));
    memcpy(run_code->ptr + run_code->size + 1, &temp_arg, sizeof(ArgCommand));
    run_code->size += 2;
    return COMP_ERROR_NO;
}

static ErrorCompilation without_arg_command(char *str, RunFile *run_code, Command command)
{
    Cmd temp = {};
    temp.command = command;
    memcpy(run_code->ptr + run_code->size, &temp, sizeof(Cmd));
    run_code->size += 1;

    ErrorCompilation error = check_correct(str, '\0');
    return error;
}

static ErrorCompilation first_comp(ArrLabels *arr_labels, ArrayPtr *array_ptr)
{
    int ip = 0;
    ErrorCompilation error = COMP_ERROR_NO;
    for (int i = 0; i < array_ptr->size; i++)
    {
        // 1. Внутренность цикла - функция
        char *ptr = (array_ptr->ptr + i)->str;
        // 2. Часть 1: продвижение указателя
        //    Часть 2: узнаем гостей так сказать
        //    Часть 3: Еще двигаем??
        Command command = comparing_commands((array_ptr->ptr + i)->str, &ptr);

        switch (command)
        {
            case PUSH:

            case POP:

            case JUMP:

            case JB:

            case JBE:

            case JA:

            case JAE:

            case JE:

            case JNE:

            case CALL:
                ip+= 2;
                break;

            case LABEL:
                error = add_label((array_ptr->ptr + i)->str, arr_labels, ip);
                if (error != COMP_ERROR_NO)
                    return error;
                break;

            default:
                ip++;
                break;
        }
    }
    return COMP_ERROR_NO;
}

static ErrorCompilation add_label(char *str, ArrLabels *arr_labels, int ip)
{
    Label label = {};

    ErrorCompilation error = create_label(str, &label, '\0');
    if (error != COMP_ERROR_NO)
        return error;

    label.number_str = ip;
    *(arr_labels->ptr + arr_labels->size) = label;
    arr_labels->size++;
    return COMP_ERROR_NO;
}

static ErrorCompilation create_label(char *str, Label *label, char sep)
{
    while (isspace(*str) != 0)
        str++;

    if (*str == sep)
        return COMP_NONE_LABEL;


    label->name = str;

    int size_label = 0;

    while (isspace(*str) == 0 && *str != sep)
    {
        str++;
        size_label++;
    }
    label->size = size_label;

    printf("name: *%s*\nsize: %d\n\n", label->name, label->size);

    ErrorCompilation error = check_correct(str, sep);
    return error;
}
                                                                               // executable_file (exec_file)
static ErrorCompilation second_comp(ArrLabels *arr_labels, ArrayPtr *array_ptr, RunFile *run_file)
{
    ErrorCompilation error = COMP_ERROR_NO;
    for (int i = 0; i < array_ptr->size; i++)
    {
        char *ptr = (array_ptr->ptr + i)->str;
        Command command = comparing_commands((array_ptr->ptr + i)->str, &ptr);
        // FIXME:!!!!!!!!! попался на копипасте =)
        switch (command)
        {
            case PUSH:
                error = push_command(ptr, run_file);
                if (error != COMP_ERROR_NO)
                    return error;
                break;

            case POP:
                error = pop_command(ptr, run_file);
                if (error != COMP_ERROR_NO)
                    return error;
                break;

            case CALL:
                error = call_command(ptr, run_file, arr_labels);
                if (error != COMP_ERROR_NO)
                    return error;
                break;

            case JUMP:

            case JB:

            case JBE:

            case JA:

            case JAE:

            case JE:

            case JNE:
                error = jump_command(ptr, run_file, arr_labels, command);
                if (error != COMP_ERROR_NO)
                    return error;
                break;

            case ADD:

            case SID:

            case MUL:

            case DIV:

            case OUT:

            case IN:

            case HTL:

            case DUMP:

            case RESET:
                error = without_arg_command(ptr, run_file, command);
                if (error != COMP_ERROR_NO)
                    return error;
                break;

            default:
                break;
        }
    }
    return COMP_ERROR_NO;
}

static Label* find_label(char *str, const ArrLabels *arr_labels)
{
    Label label = {};

    ErrorCompilation error = create_label(str, &label, '\0');
    if (error != COMP_ERROR_NO)
        return NULL;

    return search_label(arr_labels, &label);
}

static Label* search_label(const ArrLabels *arr_labels, Label *label)
{
    int number = 0;

    while (number < arr_labels->size)
    {
        while (label->size != (arr_labels->ptr + number)->size && number < arr_labels->size)
            number++;

        if (number < arr_labels->size)
        {
            if (strncmp(label->name, (arr_labels->ptr + number)->name, label->size) == 0)
                return arr_labels->ptr + number;
        }
        number++;
    }
    return NULL;
}































