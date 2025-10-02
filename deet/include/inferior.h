#include "breakpoint.h"
typedef struct _inferior inferior;
inferior *inferior_new(const char *prog_path, int argc, const char **argv, breakpoint *bp);
void inferior_continue(inferior *inf);
void inferior_backtrace(inferior *inf);
void inferior_free(inferior *inf);