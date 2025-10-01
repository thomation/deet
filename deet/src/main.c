#include <stdio.h>
#include "debuger.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <program to debug>\n", argv[0]);
        return 1;
    }
    printf("Debugging program: %s\n", argv[1]);
    debuger *dbg = debuger_new(argv);
    if (dbg == NULL)
    {
        fprintf(stderr, "Failed to create debugger instance\n");
        return 1;
    }
    debuger_run(dbg);
    debuger_free(dbg);
    return 0;
}