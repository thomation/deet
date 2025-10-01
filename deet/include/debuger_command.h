enum command
{
    CMD_RUN,
    CMD_CONTINUE,
    CMD_STEP,
    CMD_QUIT,
    CMD_INVALID
};

typedef struct _debuger_command
{
    enum command cmd_type;
    const char *cmd;
    int argc;
    char **argv;
} debuger_command;
debuger_command *debuger_command_new(const char *cmd);
void * debuger_command_free(debuger_command *cmd);