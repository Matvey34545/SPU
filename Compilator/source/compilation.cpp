#include <string.h>
#include <stdio.h>

#include "compilation.h"

static ErrorCompilation first_comp(ArrLabels *arr_labels, const ArrayPtr *array_ptr);
static ErrorCompilation second_comp(ArrLabels *arr_labels, const ArrayPtr *array_ptr, RunFile *run_file);

static ErrorCompilation format_command(const char *str, RunFile *run_code, const ExecInfoCmd *command);
static ErrorCompilation jump_command(const char *str, RunFile *run_code, const ArrLabels *arr_labels, const ExecInfoCmd *command);
static ErrorCompilation without_arg_command(const char *str, RunFile *run_code, Command command);

static bool comparing_commands(char const **src, ExecInfoCmd *exec_command);
static ErrorCompilation is_register(char const **source, ArgCommand *arg_command);
static ErrorCompilation processing_argument_string(const char* ptr_str, Cmd *cmd, ArgCommand *arg_command, Command command);
static bool is_find_const(char const **src, ArgCommand *arg_command);
static ErrorCompilation processing_expression(char const **src, Cmd *cmd, ArgCommand *arg_command);

static void skip_spaces(char const **src);
static ErrorCompilation check_correct(const char *source, char sep);
static ErrorCompilation check_format_arg(const ExecInfoCmd *command, const Cmd *cmd);

static ErrorCompilation add_label(const char *str, ArrLabels *arr_labels, int ip);
static ErrorCompilation create_label(const char *str, Label *label, char sep, bool is_add_label);
static Label* find_label(const char *str, const ArrLabels *arr_labels);
static Label* search_label(const ArrLabels *arr_labels, Label *label);

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

static ErrorCompilation first_comp(ArrLabels *arr_labels, const ArrayPtr *array_ptr)
{
    int ip = 0;
    ErrorCompilation error = COMP_ERROR_NO;
    for (int i = 0; i < array_ptr->size; i++)
    {
        const char *ptr = array_ptr->ptr[i];
        ExecInfoCmd command = {};

        if (comparing_commands(&ptr, &command))
        {
            if (command.type_arg == WHITHOUT_ARG)
                ip++;

            else
                ip += 2;
        }
        else
        {
            error = add_label(ptr, arr_labels, ip);
            if (error != COMP_ERROR_NO)
                return error;
        }
    }
    return COMP_ERROR_NO;
}

static ErrorCompilation second_comp(ArrLabels *arr_labels, const ArrayPtr *array_ptr, RunFile *run_file)
{
    ErrorCompilation error = COMP_ERROR_NO;
    for (int i = 0; i < array_ptr->size; i++)
    {
        const char *ptr = array_ptr->ptr[i];
        ExecInfoCmd command = {};

        if (comparing_commands(&ptr, &command))
        {
            switch (command.type_arg)
            {
                case WHITHOUT_ARG:
                    error = without_arg_command(ptr, run_file, command.num_command);
                    if (error != COMP_ERROR_NO)
                        return error;
                    break;

                case FORMAT_ARG:
                    error = format_command(ptr, run_file, &command);
                    if (error != COMP_ERROR_NO)
                        return error;
                    break;

                case JUMP_ARG:
                    error = jump_command(ptr, run_file, arr_labels, &command);
                    if (error != COMP_ERROR_NO)
                        return error;
                    break;
            }
        }
    }
    return COMP_ERROR_NO;
}

static ErrorCompilation add_label(const char *str, ArrLabels *arr_labels, int ip)
{
    Label label = {};

    ErrorCompilation error = create_label(str, &label, SEP, true);
    if (error != COMP_ERROR_NO)
        return error;

    label.number_str = ip;
    *(arr_labels->ptr + arr_labels->size) = label;
    arr_labels->size++;
    return COMP_ERROR_NO;
}

static ErrorCompilation create_label(const char *str, Label *label, char sep, bool is_add_label)
{
    const char LABEL_SYMBOL = ':';
    skip_spaces(&str);

    if (*str == sep)
        return COMP_NONE_LABEL;


    label->name = str;

    int size_label = 0;

    while (isspace(*str) == 0 && *str != sep && *str != LABEL_SYMBOL)
    {
        str++;
        size_label++;
    }
    label->size = size_label;

    if (is_add_label)
    {
        skip_spaces(&str);
        if (*str != LABEL_SYMBOL)
            return COMP_ERROR_IN_NAME_LABEL;

        str++;
    }
    ErrorCompilation error = check_correct(str, sep);
    return error;
}

static ErrorCompilation format_command(const char *str, RunFile *run_code, const ExecInfoCmd *command)
{
    Cmd temp = {};
    ArgCommand temp_arg = {};

    ErrorCompilation error = processing_argument_string(str, &temp, &temp_arg, command->num_command);
    if (error != COMP_ERROR_NO)
        return error;

    error = check_format_arg(command, &temp);
    if (error != COMP_ERROR_NO)
        return error;

    memcpy(run_code->ptr + run_code->size    , &temp    , sizeof(Cmd));
    memcpy(run_code->ptr + run_code->size + 1, &temp_arg, sizeof(ArgCommand));
    run_code->size += 2;
    return COMP_ERROR_NO;
}


static ErrorCompilation jump_command(const char *str, RunFile *run_code, const ArrLabels *arr_labels, const ExecInfoCmd *command)
{
    Cmd temp = {};
    ArgCommand temp_arg = {};

    Label* label = find_label(str, arr_labels);

    if (label != NULL)
    {
        temp_arg.cnst = label->number_str;
        temp.is_cnst = 1;
        temp.command = command->num_command;
    }
    else
    {
        ErrorCompilation error = processing_argument_string(str, &temp, &temp_arg, command->num_command);
        if (error != COMP_ERROR_NO)
            return error;
    }
    ErrorCompilation error = check_format_arg(command, &temp);
    if (error != COMP_ERROR_NO)
        return error;

    memcpy(run_code->ptr + run_code->size, &temp, sizeof(Cmd));
    memcpy(run_code->ptr + run_code->size + 1, &temp_arg, sizeof(ArgCommand));
    run_code->size += 2;
    return COMP_ERROR_NO;
}

static ErrorCompilation without_arg_command(const char *str, RunFile *run_code, Command command)
{
    Cmd temp = {};
    temp.command = command;
    memcpy(run_code->ptr + run_code->size, &temp, sizeof(Cmd));
    run_code->size += 1;

    ErrorCompilation error = check_correct(str, SEP);
    return error;
}

static ErrorCompilation check_format_arg(const ExecInfoCmd *command, const Cmd *cmd)
{
    if (command->is_value)
    {
        if (!(cmd->is_register || cmd->is_cnst))
            return COMP_ERROR_IN_ARG;
    }
    else
    {
        if (!cmd->is_memory)
        {
            if (cmd->is_cnst)
                return COMP_ERROR_IN_ARG;
        }
    }
    return COMP_ERROR_NO;
}

static bool comparing_commands(char const **src, ExecInfoCmd *exec_command)
{
    skip_spaces(src);

    size_t number_command = sizeof(info_commands) / sizeof(InfoCmd);
    int command = 0;

    while (command < number_command)
    {
        if (strncmp(*src, info_commands[command].name, info_commands[command].size) == 0)
        {
            if (isspace(*(*src + info_commands[command].size)) != 0 || *(*src + info_commands[command].size) == SEP)
            {
                *src += info_commands[command].size;
                memcpy(exec_command, &(info_commands[command].exec_info), sizeof(ExecInfoCmd));
                return true;
            }
        }
        command++;
    }
    return false;
}

static ErrorCompilation check_correct(const char *source, char sep)
{
    skip_spaces(&source);

    if (*source != sep)
        return COMP_MANY_ARGUMENTS;

    return COMP_ERROR_NO;
}

static void skip_spaces(char const **src)
{
    while (isspace(**src) != 0)
        (*src)++;
}

ErrorCompilation processing_argument_string(const char *ptr_str, Cmd *cmd, ArgCommand *arg_command, Command command)
{
    const char RAM_SYMBL      = '[';
    const char BACK_RAM_SYMBL = ']';
    ErrorCompilation error = COMP_ERROR_NO;

    cmd->is_memory = false;
    cmd->is_register = false;
    cmd->is_cnst = false;

    skip_spaces(&ptr_str);


    if (*ptr_str == RAM_SYMBL)
    {
        ptr_str++;
        cmd->is_memory = true;

        error = processing_expression(&ptr_str, cmd, arg_command);

        skip_spaces(&ptr_str);

        if (*ptr_str++ != BACK_RAM_SYMBL)
            return COMP_ERROR_SYNTAX;
    }
    else
        error = processing_expression(&ptr_str, cmd, arg_command);

    cmd->command = (unsigned int)command;
    error = check_correct(ptr_str, SEP);
    return error;
}

static ErrorCompilation processing_expression(char const **src, Cmd *cmd, ArgCommand *arg_command)
{
    const char PLUS_SYMBL = '+';
    ErrorCompilation error = COMP_ERROR_NO;

    if (is_find_const(src, arg_command))
            cmd->is_cnst = true;

    else
    {
        error = is_register(src, arg_command);
        if (error != COMP_ERROR_NO)
            return error;

        cmd->is_register = true;

        skip_spaces(src);

        if (**src == PLUS_SYMBL)
        {
            (*src)++;

            if (is_find_const(src, arg_command))
                cmd->is_cnst = true;

            else
                return COMP_EXPECTED_CONST;
        }
    }
    return COMP_ERROR_NO;
}

static bool is_find_const(char const **src, ArgCommand *arg_command)
{
    int read_symbol = 0;
    int cnst        = 0;
    int is_cnst = sscanf(*src, "%d%n", &cnst, &read_symbol);

    if (is_cnst >= 1)
    {
        *src += read_symbol;
        arg_command->cnst = cnst;
        return true;
    }
    return false;
}


static ErrorCompilation is_register(char const **source, ArgCommand *arg_command)
{
    skip_spaces(source);

    int number_register = sizeof(registers) / sizeof(RegName);
    int reg = 0;

    while (reg < number_register)
    {
        if (strncmp(*source, registers[reg].name, registers[reg].size) == 0)
        {
            *source += registers[reg].size;
            arg_command->num_reg = (unsigned int)reg;
            return COMP_ERROR_NO;
        }
        reg++;
    }
    return COMP_NONE_REGISTER;
}

static Label* find_label(const char *str, const ArrLabels *arr_labels)
{
    Label label = {};

    ErrorCompilation error = create_label(str, &label, SEP, false);
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































