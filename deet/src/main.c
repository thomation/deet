#include <stdio.h>
#include "debuger.h"
#include <signal.h>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <program to debug>\n", argv[0]);
        return 1;
    }
    printf("Debugging program: %s\n", argv[1]);
    debuger *dbg = debuger_new(argv[1]);
    if (dbg == NULL)
    {
        fprintf(stderr, "Failed to create debugger instance\n");
        return 1;
    }
    // Disable handling of ctrl+c in this process (so that ctrl+c only gets delivered to child
    // processes)
    signal(SIGINT, SIG_IGN);
    debuger_run(dbg);
    debuger_free(dbg);
    return 0;
}