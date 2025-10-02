typedef struct _breakpoint breakpoint;

breakpoint *breakpoint_new();
int breakpoint_add_address(breakpoint *bp, unsigned long addr);
void breakpoint_free(breakpoint *bp);