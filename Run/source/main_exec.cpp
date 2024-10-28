#include "run.h"

int main(int argc, char *argv[])
{
    char *filename = argv[1];
    RunFile runfile = {};

    read_bynary(filename, &runfile);

    run_exec_file(&runfile);
}


