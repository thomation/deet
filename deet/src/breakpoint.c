#include "breakpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dwarf.h"

#define MAX_BREAKPOINTS 32
struct breakpoint
{
    unsigned long address;
    unsigned char original_data;
};
struct _breakpoints
{
    struct breakpoint breakpoints[MAX_BREAKPOINTS];
    int num_breakpoints;
};

breakpoints *breakpoints_new()
{
    breakpoints *bp = (breakpoints *)malloc(sizeof(breakpoints));
    if (bp)
    {
        bp->num_breakpoints = 0;
        memset(bp->breakpoints, 0, sizeof(bp->breakpoints));
    }
    return bp;
}
int breakpoints_add_address(breakpoints *bp, unsigned long addr)
{
    if (bp->num_breakpoints >= MAX_BREAKPOINTS)
    {
        printf("breakpoints list full!\n");
        return 0;
    }
    bp->breakpoints[bp->num_breakpoints++].address = addr;
    printf("breakpoints added at 0x%lx\n", addr);
    return 1;
}
int breakpoints_count(breakpoints *bp)
{
    return bp ? bp->num_breakpoints : 0;
}
unsigned long breakpoints_get_address(breakpoints *bp, int index)
{
    if (bp == NULL || index < 0 || index >= bp->num_breakpoints)
    {
        return 0;
    }
    return bp->breakpoints[index].address;
}
void breakpoints_set_original_data(breakpoints *bp, int index, unsigned char data)
{
    if (bp == NULL || index < 0 || index >= bp->num_breakpoints)
    {
        return;
    }
    bp->breakpoints[index].original_data = data;
}
void breakpoints_free(breakpoints *bp)
{
    if (bp)
    {
        free(bp);
    }
}