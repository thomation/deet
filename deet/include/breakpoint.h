typedef struct _breakpoint breakpoint;

breakpoint *breakpoint_new();
int breakpoint_add_address(breakpoint *bp, unsigned long addr);
int breakpoint_count(breakpoint *bp);
unsigned long breakpoint_get_address(breakpoint *bp, int index);
void breakpoint_free(breakpoint *bp);