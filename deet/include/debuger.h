typedef struct _debuger debuger;

debuger *debuger_new(const char *prog_path);
void debuger_run(debuger *dbg);
void debuger_free(debuger *dbg);