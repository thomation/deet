enum command
{
    CMD_RUN,
    CMD_CONTINUE,
    CMD_BACKTRACE,
    CMD_QUIT,
    CMD_INVALID
};

typedef struct _debuger_command debuger_command;
debuger_command *debuger_command_new(const char *cmd);
int debuger_command_get_args(const debuger_command *cmd, const char ***argv);
int debuger_command_get_type(const debuger_command *cmd);
void *debuger_command_free(debuger_command *cmd);