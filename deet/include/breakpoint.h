typedef struct _breakpoints breakpoints;

breakpoints *breakpoints_new();
int breakpoints_add_address(breakpoints *bp, unsigned long addr);
int breakpoints_count(breakpoints *bp);
unsigned long breakpoints_get_address(breakpoints *bp, int index);
void breakpoints_set_original_data(breakpoints *bp, int index, unsigned char data);
unsigned char breakpoints_get_original_data(breakpoints *bp, unsigned long addr);
void breakpoints_free(breakpoints *bp);