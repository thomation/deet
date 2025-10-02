#include <stdlib.h>
#include <string.h>
#include "debuger_command.h"

struct _debuger_command
{
    enum command cmd_type;
    const char *cmd;
    int argc;
    char **argv;
};
typedef struct
{
    const char *name;
    int cmd_type;
} cmd_map_entry;

static const cmd_map_entry cmd_map[] = {
    {"run", CMD_RUN},
    {"r", CMD_RUN},
    {"continue", CMD_CONTINUE},
    {"c", CMD_CONTINUE},
    {"backtrace", CMD_BACKTRACE},
    {"bt", CMD_BACKTRACE},
    {"break", CMD_BREAK},
    {"b", CMD_BREAK},
    {"quit", CMD_QUIT},
    {"q", CMD_QUIT},
};

static int lookup_cmd_type(const char *cmd)
{
    for (size_t i = 0; i < sizeof(cmd_map) / sizeof(cmd_map[0]); ++i)
    {
        if (strcmp(cmd, cmd_map[i].name) == 0)
            return cmd_map[i].cmd_type;
    }
    return CMD_INVALID;
}

debuger_command *debuger_command_new(const char *input)
{
    debuger_command *dbg_cmd = (debuger_command *)malloc(sizeof(debuger_command));
    if (dbg_cmd == NULL)
    {
        return NULL;
    }

    char *input_copy = strdup(input);
    if (!input_copy)
    {
        free(dbg_cmd);
        return NULL;
    }

    char *token = strtok(input_copy, " ");
    dbg_cmd->cmd = token ? strdup(token) : NULL;
    dbg_cmd->cmd_type = token ? lookup_cmd_type(token) : CMD_INVALID;

    dbg_cmd->argc = 0;
    int max_args = 10;
    dbg_cmd->argv = malloc(sizeof(char *) * max_args);
    char *arg;
    while ((arg = strtok(NULL, " ")) != NULL && dbg_cmd->argc < max_args)
    {
        dbg_cmd->argv[dbg_cmd->argc++] = strdup(arg);
    }

    free(input_copy);
    return dbg_cmd;
}
int debuger_command_get_args(const debuger_command *cmd, const char ***argv)
{
    if (!cmd || !argv)
        return 0;
    *argv = (const char **)cmd->argv;
    return cmd->argc;
}
int debuger_command_get_type(const debuger_command *cmd)
{
    if (!cmd)
        return CMD_INVALID;
    return cmd->cmd_type;
}
void *debuger_command_free(debuger_command *cmd)
{
    if (cmd)
    {
        if (cmd->cmd)
            free((void *)cmd->cmd);
        for (int i = 0; i < cmd->argc; i++)
        {
            if (cmd->argv[i])
                free(cmd->argv[i]);
        }
        free(cmd->argv);
        free(cmd);
    }
    return NULL;
}