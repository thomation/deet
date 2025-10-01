
typedef struct _inferior inferior;
inferior *inferior_new(const char *prog_path, int argc, const char **argv);
void inferior_free(inferior *inf);