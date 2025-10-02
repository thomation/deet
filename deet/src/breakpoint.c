#include "breakpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BREAKPOINTS 32
struct _breakpoint
{
    unsigned long breakpoints[MAX_BREAKPOINTS];
    int num_breakpoints;
};

breakpoint *breakpoint_new()
{
    breakpoint *bp = (breakpoint *)malloc(sizeof(breakpoint));
    if (bp)
    {
        bp->num_breakpoints = 0;
        memset(bp->breakpoints, 0, sizeof(bp->breakpoints));
    }
    return bp;
}
int breakpoint_add_address(breakpoint *bp, unsigned long addr)
{
    if (bp->num_breakpoints >= MAX_BREAKPOINTS)
    {
        printf("Breakpoint list full!\n");
        return 0;
    }
    bp->breakpoints[bp->num_breakpoints++] = addr;
    printf("Breakpoint added at 0x%lx\n", addr);
    return 1;
}
void breakpoint_free(breakpoint *bp)
{
    if (bp)
    {
        free(bp);
    }
}